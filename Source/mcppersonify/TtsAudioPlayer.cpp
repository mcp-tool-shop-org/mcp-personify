#include "TtsAudioPlayer.h"
#include "Sound/SoundWaveProcedural.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Kismet/GameplayStatics.h"

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

	FString Extension = FPaths::GetExtension(NormalizedPath).ToLower();

	if (Extension == TEXT("wav"))
	{
		PlayWav(NormalizedPath);
	}
	else if (Extension == TEXT("ogg"))
	{
		PlayOgg(NormalizedPath);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[TtsAudioPlayer] Unsupported format: %s"), *Extension);
	}
}

void UTtsAudioPlayer::PlayWav(const FString& FilePath)
{
	// Load raw WAV file bytes
	TArray<uint8> RawBytes;
	if (!FFileHelper::LoadFileToArray(RawBytes, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("[TtsAudioPlayer] Failed to read: %s"), *FilePath);
		return;
	}

	if (RawBytes.Num() < 44)
	{
		UE_LOG(LogTemp, Error, TEXT("[TtsAudioPlayer] WAV too small: %d bytes"), RawBytes.Num());
		return;
	}

	// Parse WAV header
	int32 NumChannels = RawBytes[22] | (RawBytes[23] << 8);
	int32 SampleRate = RawBytes[24] | (RawBytes[25] << 8) | (RawBytes[26] << 16) | (RawBytes[27] << 24);
	int32 BitsPerSample = RawBytes[34] | (RawBytes[35] << 8);

	// Find "data" chunk
	int32 DataOffset = 44; // Standard WAV
	int32 DataSize = RawBytes.Num() - DataOffset;

	// Search for "data" marker in case of non-standard headers
	for (int32 i = 12; i < FMath::Min(RawBytes.Num() - 8, 200); i++)
	{
		if (RawBytes[i] == 'd' && RawBytes[i+1] == 'a' && RawBytes[i+2] == 't' && RawBytes[i+3] == 'a')
		{
			DataSize = RawBytes[i+4] | (RawBytes[i+5] << 8) | (RawBytes[i+6] << 16) | (RawBytes[i+7] << 24);
			DataOffset = i + 8;
			break;
		}
	}

	if (DataOffset + DataSize > RawBytes.Num())
	{
		DataSize = RawBytes.Num() - DataOffset;
	}

	// Use SoundWaveProcedural — feed raw PCM data directly
	USoundWaveProcedural* SoundWave = NewObject<USoundWaveProcedural>();
	SoundWave->SetSampleRate(SampleRate);
	SoundWave->NumChannels = NumChannels;
	SoundWave->Duration = (float)DataSize / (SampleRate * NumChannels * (BitsPerSample / 8));
	SoundWave->SoundGroup = ESoundGroup::SOUNDGROUP_Voice;
	SoundWave->bLooping = false;

	// Queue the raw PCM audio data
	SoundWave->QueueAudio(RawBytes.GetData() + DataOffset, DataSize);

	AudioComp->SetSound(SoundWave);
	AudioComp->Play();

	UE_LOG(LogTemp, Log, TEXT("[TtsAudioPlayer] Playing WAV: %s (%.1fs, %dHz, %dch)"),
		*FPaths::GetCleanFilename(FilePath), SoundWave->Duration, SampleRate, NumChannels);
	OnPlaybackStarted.Broadcast(AudioComp);
}

void UTtsAudioPlayer::PlayOgg(const FString& FilePath)
{
	// For OGG files, use RuntimeAudioImporter or decode manually.
	// As a simple approach, we use UGameplayStatics::SpawnSound2D with
	// a SoundWave created from the OGG data.
	//
	// UE5's built-in OGG support works through the import pipeline,
	// but runtime loading of OGG requires either:
	// 1. Converting to WAV first (bridge can output WAV)
	// 2. Using a runtime audio import plugin
	// 3. Using USoundWaveProcedural with decoded PCM
	//
	// For now, log a message suggesting WAV format from the bridge.

	UE_LOG(LogTemp, Warning, TEXT("[TtsAudioPlayer] OGG runtime loading not yet implemented. "
		"Set TTS_BRIDGE_FORMAT=wav environment variable for the bridge, or use WAV output."));

	// Try loading as WAV anyway in case the extension is wrong
	// (some TTS engines output WAV with .ogg extension)
	TArray<uint8> RawBytes;
	if (FFileHelper::LoadFileToArray(RawBytes, *FilePath) && RawBytes.Num() >= 4)
	{
		// Check for RIFF header (WAV)
		if (RawBytes[0] == 'R' && RawBytes[1] == 'I' && RawBytes[2] == 'F' && RawBytes[3] == 'F')
		{
			UE_LOG(LogTemp, Log, TEXT("[TtsAudioPlayer] File has RIFF header, treating as WAV"));
			PlayWav(FilePath);
			return;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("[TtsAudioPlayer] Cannot play OGG: %s"), *FilePath);
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
