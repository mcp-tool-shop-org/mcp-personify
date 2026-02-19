#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BridgeClient.h"
#include "TtsAudioPlayer.h"
#include "VisemeDriverFFT.h"
#include "ExpressionMapper.h"
#include "VrmAvatarLoader.h"
#include "PersonifyManager.generated.h"

/**
 * Main orchestrator actor. Place one in the level.
 * Wires BridgeClient -> AudioPlayer -> VisemeDriver -> ExpressionMapper.
 * Auto-starts the TTS bridge process if needed.
 */
UCLASS()
class MCPPERSONIFY_API APersonifyManager : public AActor
{
	GENERATED_BODY()

public:
	APersonifyManager();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UBridgeClient* BridgeClient;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UTtsAudioPlayer* AudioPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UVisemeDriverFFT* VisemeDriver;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UExpressionMapper* ExpressionMapper;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UVrmAvatarLoader* AvatarLoader;

	// --- Bridge Auto-Start ---
	UPROPERTY(EditAnywhere, Category="Bridge") bool bAutoStartBridge = true;
	UPROPERTY(EditAnywhere, Category="Bridge") float RetryDelay = 2.0f;
	UPROPERTY(EditAnywhere, Category="Bridge") int32 MaxRetries = 5;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	FProcHandle BridgeProcess;
	int32 RetryCount = 0;
	FTimerHandle RetryTimerHandle;

	void WireComponents();
	void TryConnect();
	void SpawnBridge();
	void CheckConnection();

	UFUNCTION() void OnBridgeConnected();
	UFUNCTION() void OnBridgeDisconnected();
	UFUNCTION() void OnTtsPlay(const FTtsPlayMessage& Message);
	UFUNCTION() void OnPlaybackStarted(UAudioComponent* AudioComp);
	UFUNCTION() void OnPlaybackFinished();
	UFUNCTION() void OnVisemesReceived(float Aa, float Ih, float Ou, float Ee, float Oh);
	UFUNCTION() void OnAsideCue(const FAsideCue& Cue);
	UFUNCTION() void OnAvatarLoaded(AActor* AvatarActor);
};
