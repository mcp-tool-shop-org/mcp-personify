#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/AudioComponent.h"
#include "VisemeDriverFFT.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnVisemes, float, Aa, float, Ih, float, Ou, float, Ee, float, Oh);

/**
 * Performs FFT on the active AudioComponent each frame and extracts
 * 5 viseme band energies: aa, ih, ou, ee, oh.
 * Uses asymmetric exponential smoothing (fast attack, slow release).
 */
UCLASS(ClassGroup=(Personify), meta=(BlueprintSpawnableComponent))
class MCPPERSONIFY_API UVisemeDriverFFT : public UActorComponent
{
	GENERATED_BODY()

public:
	UVisemeDriverFFT();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FFT") float AttackMs = 60.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FFT") float ReleaseMs = 120.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FFT") float ScaleFactor = 50.f;

	UPROPERTY(BlueprintAssignable, Category="Viseme") FOnVisemes OnVisemes;

	/** Set the audio component to analyze. Null to stop. */
	UFUNCTION(BlueprintCallable, Category="Viseme")
	void SetActiveSource(UAudioComponent* Source);

	/** Current smoothed viseme values (read-only). */
	UPROPERTY(BlueprintReadOnly, Category="Viseme") float Aa = 0.f;
	UPROPERTY(BlueprintReadOnly, Category="Viseme") float Ih = 0.f;
	UPROPERTY(BlueprintReadOnly, Category="Viseme") float Ou = 0.f;
	UPROPERTY(BlueprintReadOnly, Category="Viseme") float Ee = 0.f;
	UPROPERTY(BlueprintReadOnly, Category="Viseme") float Oh = 0.f;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY() UAudioComponent* ActiveSource = nullptr;

	// Frequency band ranges (Hz)
	FVector2D BandAa = FVector2D(700, 1100);
	FVector2D BandIh = FVector2D(300, 700);
	FVector2D BandOu = FVector2D(200, 400);
	FVector2D BandEe = FVector2D(1200, 2500);
	FVector2D BandOh = FVector2D(400, 800);

	float SmoothValue(float Current, float Target, float DeltaTime) const;
};
