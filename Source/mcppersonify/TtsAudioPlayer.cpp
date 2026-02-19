#include "TtsAudioPlayer.h"
#include "Sound/SoundWaveProcedural.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Kismet/GameplayStatics.h"
#include "AudioDecompress.h"

UTtsAudioPlayer::UTtsAudioPlayer()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTtsAudioPlayer::BeginPlay()
{
	Super::BeginPlay();

	// Create a persistent AudioComponent on our owner
	AudioComp = NewObject<UAudioComponent>(GetOwner(), TEXT("PersonifyAudio"));
	AudioComp->bAutoActivate = false;
	AudioComp->bIsUISound = true; // 2D audio
	AudioComp->RegisterComponent();
	AudioComp->AttachToComponent(GetOwner()->GetRootComponent(),
		FAttachmentTransformRules::KeepRelativeTransform);

	AudioComp->OnAudioFinished.AddDynamic(this, &UTtsAudioPlayer::HandlePlaybackFinished);
}

bool UTtsAudioPlayer::IsPlaying() const
{
	return AudioComp && AudioComp->IsPlaying();
}

void UTtsAudioPlayer::PlayFromPath(const FString& FilePath)
{
	if (FilePath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[TtsAudioPlayer] Empty file path"));
		return;
	}

	// Stop any current playback
	if (AudioComp && AudioComp->IsPlaying())
	{
		AudioComp->Stop();
	}

	// Normalize path
	FString NormalizedPath = FilePath;
	FPaths::NormalizeFilename(NormalizedPath);

	if (!FPaths::FileExists(NormalizedPath))
	{
		UE_LOG(LogTemp, Error, TEXT("[TtsAudioPlayer] File not found: %s"), *NormalizedPath);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[TtsAudioPlayer] Loading: %s"), *NormalizedPath);

	// Load raw bytes
	TArray<uint8> RawBytes;
	if (!FFileHelper::LoadFileToArray(RawBytes, *NormalizedPath))
	{
		UE_LOG(LogTemp, Error, TEXT("[TtsAudioPlayer] Failed to read: %s"), *NormalizedPath);
		return;
	}

	// Create a SoundWave from raw file data
	USoundWave* SoundWave = NewObject<USoundWave>();
	SoundWave->RawData.Lock(LOCK_READ_WRITE);
	void* LockedData = SoundWave->RawData.Realloc(RawBytes.Num());
	FMemory::Memcpy(LockedData, RawBytes.GetData(), RawBytes.Num());
	SoundWave->RawData.Unlock();

	// Detect format from extension
	FString Extension = FPaths::GetExtension(NormalizedPath).ToLower();
	if (Extension == TEXT("wav"))
	{
		// Parse WAV header for metadata
		if (RawBytes.Num() >= 44)
		{
			int32 NumChannels = RawBytes[22] | (RawBytes[23] << 8);
			int32 SRate = RawBytes[24] | (RawBytes[25] << 8) | (RawBytes[26] << 16) | (RawBytes[27] << 24);
			int32 BitsPerSample = RawBytes[34] | (RawBytes[35] << 8);

			SoundWave->SetSampleRate(SRate);
			SoundWave->NumChannels = NumChannels;
			SoundWave->RawPCMDataSize = RawBytes.Num() - 44;
			SoundWave->Duration = (float)SoundWave->RawPCMDataSize / (SRate * NumChannels * (BitsPerSample / 8));
		}
		SoundWave->SetImportedSampleRate(24000);
		SoundWave->SetSampleRate(24000);
		SoundWave->NumChannels = 1;
	}
	else if (Extension == TEXT("ogg"))
	{
		// OGG Vorbis — let UE decode it
		SoundWave->SetImportedSampleRate(24000);
		SoundWave->SetSampleRate(24000);
		SoundWave->NumChannels = 1;
		SoundWave->bProcedural = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[TtsAudioPlayer] Unsupported format: %s"), *Extension);
		return;
	}

	SoundWave->SoundGroup = ESoundGroup::SOUNDGROUP_Voice;
	SoundWave->bLooping = false;

	AudioComp->SetSound(SoundWave);
	AudioComp->Play();

	UE_LOG(LogTemp, Log, TEXT("[TtsAudioPlayer] Playing: %s (%.1fs)"), *FPaths::GetCleanFilename(NormalizedPath), SoundWave->Duration);
	OnPlaybackStarted.Broadcast(AudioComp);
}

void UTtsAudioPlayer::Stop()
{
	if (AudioComp && AudioComp->IsPlaying())
	{
		AudioComp->Stop();
		OnPlaybackFinished.Broadcast();
	}
}

void UTtsAudioPlayer::HandlePlaybackFinished()
{
	UE_LOG(LogTemp, Log, TEXT("[TtsAudioPlayer] Playback finished"));
	OnPlaybackFinished.Broadcast();
}
