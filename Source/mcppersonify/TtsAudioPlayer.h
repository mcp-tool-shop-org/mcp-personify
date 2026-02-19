#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundWave.h"
#include "TtsAudioPlayer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlaybackStarted, UAudioComponent*, AudioComp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlaybackFinished);

/**
 * Loads WAV/OGG audio files from disk and plays them.
 * Receives file paths from the TTS bridge via BridgeClient.
 */
UCLASS(ClassGroup=(Personify), meta=(BlueprintSpawnableComponent))
class MCPPERSONIFY_API UTtsAudioPlayer : public UActorComponent
{
	GENERATED_BODY()

public:
	UTtsAudioPlayer();

	UPROPERTY(BlueprintAssignable, Category="Audio") FOnPlaybackStarted OnPlaybackStarted;
	UPROPERTY(BlueprintAssignable, Category="Audio") FOnPlaybackFinished OnPlaybackFinished;

	UFUNCTION(BlueprintPure, Category="Audio")
	bool IsPlaying() const;

	/** Load and play an audio file from a disk path (WAV or OGG). */
	UFUNCTION(BlueprintCallable, Category="Audio")
	void PlayFromPath(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category="Audio")
	void Stop();

	/** Get the active AudioComponent (for FFT analysis). */
	UFUNCTION(BlueprintPure, Category="Audio")
	UAudioComponent* GetAudioComponent() const { return AudioComp; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY() UAudioComponent* AudioComp = nullptr;

	USoundWave* LoadWavFromFile(const FString& FilePath);

	UFUNCTION()
	void HandlePlaybackFinished();
};
