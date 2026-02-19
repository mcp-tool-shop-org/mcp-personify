#include "PersonifyManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "TimerManager.h"

APersonifyManager::APersonifyManager()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create default sub-objects
	BridgeClient = CreateDefaultSubobject<UBridgeClient>(TEXT("BridgeClient"));
	AudioPlayer = CreateDefaultSubobject<UTtsAudioPlayer>(TEXT("AudioPlayer"));
	VisemeDriver = CreateDefaultSubobject<UVisemeDriverFFT>(TEXT("VisemeDriver"));
	ExpressionMapper = CreateDefaultSubobject<UExpressionMapper>(TEXT("ExpressionMapper"));
	AvatarLoader = CreateDefaultSubobject<UVrmAvatarLoader>(TEXT("AvatarLoader"));
}

void APersonifyManager::BeginPlay()
{
	Super::BeginPlay();
	WireComponents();

	if (bAutoStartBridge)
	{
		TryConnect();
	}
}

void APersonifyManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(RetryTimerHandle);

	// Kill bridge process if we spawned it
	if (BridgeProcess.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("[PersonifyManager] Killing bridge process"));
		FPlatformProcess::TerminateProc(BridgeProcess, true);
		BridgeProcess.Reset();
	}

	Super::EndPlay(EndPlayReason);
}

void APersonifyManager::WireComponents()
{
	// Bridge -> AudioPlayer: play TTS audio
	BridgeClient->OnTtsPlay.AddDynamic(this, &APersonifyManager::OnTtsPlay);

	// AudioPlayer -> VisemeDriver: set active source
	AudioPlayer->OnPlaybackStarted.AddDynamic(this, &APersonifyManager::OnPlaybackStarted);
	AudioPlayer->OnPlaybackFinished.AddDynamic(this, &APersonifyManager::OnPlaybackFinished);

	// VisemeDriver -> ExpressionMapper: route visemes
	VisemeDriver->OnVisemes.AddDynamic(this, &APersonifyManager::OnVisemesReceived);

	// Bridge aside cues -> expression
	BridgeClient->OnAsideCue.AddDynamic(this, &APersonifyManager::OnAsideCue);

	// Bridge connection events
	BridgeClient->OnConnected.AddDynamic(this, &APersonifyManager::OnBridgeConnected);
	BridgeClient->OnDisconnected.AddDynamic(this, &APersonifyManager::OnBridgeDisconnected);

	// Avatar loaded -> bind expression mapper
	AvatarLoader->OnAvatarLoaded.AddDynamic(this, &APersonifyManager::OnAvatarLoaded);

	UE_LOG(LogTemp, Log, TEXT("[PersonifyManager] All components wired"));
}

// --- Bridge Auto-Start ---

void APersonifyManager::TryConnect()
{
	UE_LOG(LogTemp, Log, TEXT("[PersonifyManager] Attempting bridge connection..."));
	BridgeClient->Connect();

	// Check connection after delay
	GetWorldTimerManager().SetTimer(RetryTimerHandle, this,
		&APersonifyManager::CheckConnection, RetryDelay, false);
}

void APersonifyManager::CheckConnection()
{
	if (BridgeClient->IsConnected())
	{
		UE_LOG(LogTemp, Log, TEXT("[PersonifyManager] Bridge already running"));
		return;
	}

	// Spawn bridge process if not already running
	if (!BridgeProcess.IsValid())
	{
		SpawnBridge();
	}

	RetryCount++;
	if (RetryCount < MaxRetries)
	{
		UE_LOG(LogTemp, Log, TEXT("[PersonifyManager] Retry %d/%d..."), RetryCount, MaxRetries);
		BridgeClient->Disconnect();

		// Retry after delay
		GetWorldTimerManager().SetTimer(RetryTimerHandle, this,
			&APersonifyManager::TryConnect, RetryDelay, false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[PersonifyManager] Max retries reached. Bridge not available."));
	}
}

void APersonifyManager::SpawnBridge()
{
	FString BridgePath = FPaths::Combine(FPaths::ProjectDir(), TEXT("tools"), TEXT("tts-bridge"), TEXT("bridge.mjs"));
	BridgePath = FPaths::ConvertRelativePathToFull(BridgePath);

	if (!FPaths::FileExists(BridgePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("[PersonifyManager] Bridge script not found: %s"), *BridgePath);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[PersonifyManager] Spawning bridge: node %s"), *BridgePath);

	FString Args = FString::Printf(TEXT("\"%s\""), *BridgePath);
	FString WorkingDir = FPaths::GetPath(BridgePath);

	uint32 ProcessID;
	BridgeProcess = FPlatformProcess::CreateProc(
		TEXT("node"),         // Executable
		*Args,                // Arguments
		false,                // bLaunchDetached
		true,                 // bLaunchHidden
		true,                 // bLaunchReallyHidden
		&ProcessID,           // OutProcessID
		0,                    // PriorityModifier
		*WorkingDir,          // WorkingDirectory
		nullptr               // PipeWriteChild
	);

	if (BridgeProcess.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("[PersonifyManager] Bridge process started (PID %u)"), ProcessID);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[PersonifyManager] Failed to start bridge process. Is Node.js installed?"));
	}
}

// --- Event Handlers ---

void APersonifyManager::OnBridgeConnected()
{
	RetryCount = 0;
	BridgeClient->SendStatus();
	UE_LOG(LogTemp, Log, TEXT("[PersonifyManager] Bridge connected, requesting voice list"));
}

void APersonifyManager::OnBridgeDisconnected()
{
	UE_LOG(LogTemp, Log, TEXT("[PersonifyManager] Bridge disconnected, attempting reconnect..."));
	RetryCount = 0;
	GetWorldTimerManager().SetTimer(RetryTimerHandle, this,
		&APersonifyManager::TryConnect, RetryDelay, false);
}

void APersonifyManager::OnTtsPlay(const FTtsPlayMessage& Message)
{
	AudioPlayer->PlayFromPath(Message.Path);

	if (!Message.Emotion.IsEmpty())
	{
		ExpressionMapper->SetEmotion(Message.Emotion, Message.Intensity);
	}
}

void APersonifyManager::OnPlaybackStarted(UAudioComponent* AudioComp)
{
	VisemeDriver->SetActiveSource(AudioComp);
}

void APersonifyManager::OnPlaybackFinished()
{
	VisemeDriver->SetActiveSource(nullptr);
	ExpressionMapper->ClearEmotion();
}

void APersonifyManager::OnVisemesReceived(float Aa, float Ih, float Ou, float Ee, float Oh)
{
	ExpressionMapper->ApplyVisemes(Aa, Ih, Ou, Ee, Oh);
}

void APersonifyManager::OnAsideCue(const FAsideCue& Cue)
{
	ExpressionMapper->SetEmotion(Cue.Emotion, Cue.Intensity);
}

void APersonifyManager::OnAvatarLoaded(AActor* AvatarActor)
{
	ExpressionMapper->BindAvatar(AvatarActor);
	UE_LOG(LogTemp, Log, TEXT("[PersonifyManager] Avatar loaded and bound to expression mapper"));
}
