#include "ExpressionMapper.h"

UExpressionMapper::UExpressionMapper()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UExpressionMapper::BeginPlay()
{
	Super::BeginPlay();
	if (Profile.ProfileName.IsEmpty())
	{
		LoadDefaultProfile();
	}
}

void UExpressionMapper::LoadDefaultProfile()
{
	Profile.ProfileName = TEXT("VRM Standard");
	Profile.Viseme_AA = TEXT("Fcl_MTH_A");
	Profile.Viseme_IH = TEXT("Fcl_MTH_I");
	Profile.Viseme_OU = TEXT("Fcl_MTH_U");
	Profile.Viseme_EE = TEXT("Fcl_MTH_E");
	Profile.Viseme_OH = TEXT("Fcl_MTH_O");
	Profile.BlinkLeft = TEXT("Fcl_EYE_Close_L");
	Profile.BlinkRight = TEXT("Fcl_EYE_Close_R");
	Profile.Happy = TEXT("Fcl_ALL_Fun");
	Profile.Sad = TEXT("Fcl_ALL_Sorrow");
	Profile.Angry = TEXT("Fcl_ALL_Angry");
	Profile.Surprised = TEXT("Fcl_ALL_Surprised");
}

void UExpressionMapper::BindAvatar(AActor* AvatarActor)
{
	Cache.Build(AvatarActor);
	bBound = Cache.Count() > 0;

	CurrentEmotion = TEXT("");
	CurrentIntensity = 0.f;
	TargetIntensity = 0.f;
	bEmotionActive = false;

	// Diagnostic: check mappings
	CheckMapping(TEXT("viseme_aa"), Profile.Viseme_AA);
	CheckMapping(TEXT("viseme_ih"), Profile.Viseme_IH);
	CheckMapping(TEXT("viseme_ou"), Profile.Viseme_OU);
	CheckMapping(TEXT("viseme_ee"), Profile.Viseme_EE);
	CheckMapping(TEXT("viseme_oh"), Profile.Viseme_OH);
	CheckMapping(TEXT("happy"), Profile.Happy);
	CheckMapping(TEXT("sad"), Profile.Sad);
	CheckMapping(TEXT("angry"), Profile.Angry);
	CheckMapping(TEXT("surprised"), Profile.Surprised);
}

void UExpressionMapper::ApplyVisemes(float Aa, float Ih, float Ou, float Ee, float Oh)
{
	if (!bBound) return;

	if (!Profile.Viseme_AA.IsEmpty()) Cache.Set(Profile.Viseme_AA, Aa);
	if (!Profile.Viseme_IH.IsEmpty()) Cache.Set(Profile.Viseme_IH, Ih);
	if (!Profile.Viseme_OU.IsEmpty()) Cache.Set(Profile.Viseme_OU, Ou);
	if (!Profile.Viseme_EE.IsEmpty()) Cache.Set(Profile.Viseme_EE, Ee);
	if (!Profile.Viseme_OH.IsEmpty()) Cache.Set(Profile.Viseme_OH, Oh);
}

void UExpressionMapper::SetEmotion(const FString& Emotion, float Intensity)
{
	// Clear previous if switching
	if (!CurrentEmotion.IsEmpty() && CurrentEmotion != Emotion)
	{
		FString PrevShape = GetEmotionBlendShape(CurrentEmotion);
		if (!PrevShape.IsEmpty() && bBound)
		{
			Cache.Set(PrevShape, 0.f);
		}
	}

	CurrentEmotion = Emotion;
	TargetIntensity = FMath::Clamp(Intensity, 0.f, 1.f);
	bEmotionActive = true;
}

void UExpressionMapper::ClearEmotion()
{
	TargetIntensity = 0.f;
	// bEmotionActive stays true so Tick can ramp down
}

void UExpressionMapper::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bBound || !bEmotionActive) return;

	float AttackRate = 1.f - FMath::Exp(-DeltaTime * 1000.f / EmotionAttackMs);
	float ReleaseRate = 1.f - FMath::Exp(-DeltaTime * 1000.f / EmotionReleaseMs);
	float Rate = TargetIntensity > CurrentIntensity ? AttackRate : ReleaseRate;
	CurrentIntensity = FMath::Lerp(CurrentIntensity, TargetIntensity, Rate);

	FString BlendShapeName = GetEmotionBlendShape(CurrentEmotion);
	if (!BlendShapeName.IsEmpty())
	{
		Cache.Set(BlendShapeName, CurrentIntensity);
	}

	// If released and near zero, stop
	if (TargetIntensity < 0.01f && CurrentIntensity < 0.01f)
	{
		bEmotionActive = false;
		CurrentIntensity = 0.f;
		if (!BlendShapeName.IsEmpty())
			Cache.Set(BlendShapeName, 0.f);
	}
}

FString UExpressionMapper::GetEmotionBlendShape(const FString& Emotion) const
{
	FString Lower = Emotion.ToLower();
	if (Lower == TEXT("happy") || Lower == TEXT("joy")) return Profile.Happy;
	if (Lower == TEXT("sad") || Lower == TEXT("sorrow")) return Profile.Sad;
	if (Lower == TEXT("angry") || Lower == TEXT("anger")) return Profile.Angry;
	if (Lower == TEXT("surprised") || Lower == TEXT("surprise")) return Profile.Surprised;
	return TEXT("");
}

void UExpressionMapper::CheckMapping(const FString& Label, const FString& BlendShapeName) const
{
	if (BlendShapeName.IsEmpty()) return;
	if (!Cache.Has(BlendShapeName))
	{
		UE_LOG(LogTemp, Warning, TEXT("[ExpressionMapper] Mapping '%s' -> '%s' NOT FOUND on avatar"),
			*Label, *BlendShapeName);
	}
}
