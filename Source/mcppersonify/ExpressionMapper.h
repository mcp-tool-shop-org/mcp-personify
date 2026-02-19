#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlendShapeCache.h"
#include "ExpressionMapper.generated.h"

/** Blend shape mapping profile loaded from JSON. */
USTRUCT(BlueprintType)
struct FMappingProfile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) FString ProfileName;
	UPROPERTY(EditAnywhere) FString Viseme_AA;
	UPROPERTY(EditAnywhere) FString Viseme_IH;
	UPROPERTY(EditAnywhere) FString Viseme_OU;
	UPROPERTY(EditAnywhere) FString Viseme_EE;
	UPROPERTY(EditAnywhere) FString Viseme_OH;
	UPROPERTY(EditAnywhere) FString BlinkLeft;
	UPROPERTY(EditAnywhere) FString BlinkRight;
	UPROPERTY(EditAnywhere) FString Happy;
	UPROPERTY(EditAnywhere) FString Sad;
	UPROPERTY(EditAnywhere) FString Angry;
	UPROPERTY(EditAnywhere) FString Surprised;
};

/**
 * Maps viseme values and emotion names to morph target names via a mapping profile.
 * Handles smoothing for emotion ramp on/off.
 */
UCLASS(ClassGroup=(Personify), meta=(BlueprintSpawnableComponent))
class MCPPERSONIFY_API UExpressionMapper : public UActorComponent
{
	GENERATED_BODY()

public:
	UExpressionMapper();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mapping")
	FMappingProfile Profile;

	UPROPERTY(EditAnywhere, Category="Smoothing") float EmotionAttackMs = 300.f;
	UPROPERTY(EditAnywhere, Category="Smoothing") float EmotionReleaseMs = 1000.f;

	/** Bind to a new avatar. Call after loading VRM. */
	UFUNCTION(BlueprintCallable, Category="Expression")
	void BindAvatar(AActor* AvatarActor);

	/** Apply viseme weights from FFT (called every frame). */
	UFUNCTION(BlueprintCallable, Category="Expression")
	void ApplyVisemes(float Aa, float Ih, float Ou, float Ee, float Oh);

	/** Set emotion with ramp. */
	UFUNCTION(BlueprintCallable, Category="Expression")
	void SetEmotion(const FString& Emotion, float Intensity);

	/** Ramp emotion to zero. */
	UFUNCTION(BlueprintCallable, Category="Expression")
	void ClearEmotion();

	/** Load default VRM Standard profile. */
	void LoadDefaultProfile();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FBlendShapeCache Cache;
	bool bBound = false;

	FString CurrentEmotion;
	float TargetIntensity = 0.f;
	float CurrentIntensity = 0.f;
	bool bEmotionActive = false;

	FString GetEmotionBlendShape(const FString& Emotion) const;
	void CheckMapping(const FString& Label, const FString& BlendShapeName) const;
};
