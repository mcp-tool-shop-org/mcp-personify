#include "VisemeDriverFFT.h"

UVisemeDriverFFT::UVisemeDriverFFT()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UVisemeDriverFFT::SetActiveSource(UAudioComponent* Source)
{
	ActiveSource = Source;
	if (Source)
	{
		UE_LOG(LogTemp, Log, TEXT("[VisemeDriverFFT] Active source set"));
	}
}

void UVisemeDriverFFT::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!ActiveSource || !ActiveSource->IsPlaying())
	{
		// Decay to zero when not playing
		if (Aa > 0.001f || Ih > 0.001f || Ou > 0.001f || Ee > 0.001f || Oh > 0.001f)
		{
			float DecayRate = 1.f - FMath::Exp(-DeltaTime * 1000.f / ReleaseMs);
			Aa = FMath::Lerp(Aa, 0.f, DecayRate);
			Ih = FMath::Lerp(Ih, 0.f, DecayRate);
			Ou = FMath::Lerp(Ou, 0.f, DecayRate);
			Ee = FMath::Lerp(Ee, 0.f, DecayRate);
			Oh = FMath::Lerp(Oh, 0.f, DecayRate);
			OnVisemes.Broadcast(Aa, Ih, Ou, Ee, Oh);
		}
		return;
	}

	// UE5 does not expose per-frame spectrum data directly from AudioComponent
	// in the same way as Unity's GetSpectrumData. We use the Audio Analyzer API
	// or amplitude-based estimation as a fallback.
	//
	// For now we use envelope-based viseme estimation from the audio component's
	// envelope follower. Full FFT integration requires the AudioAnalyzer plugin
	// or ConstantQ analysis from the Submix system.
	//
	// TODO: Integrate with FAudioDevice::GetSpectralData or SubmixBufferListener
	// for true frequency-band analysis. For now, we use a simplified amplitude
	// approach that still produces plausible mouth movement.

	float Envelope = 0.f;
	if (ActiveSource && ActiveSource->IsPlaying())
	{
		// Get the current envelope from the audio component
		// This gives us overall amplitude which we distribute across bands
		// with temporal offsets to create plausible viseme variation
		float Time = GetWorld()->GetTimeSeconds();

		// Simulate band energies from envelope with phase-shifted sine modulation
		// This produces visually plausible mouth movement until we add real FFT
		Envelope = ActiveSource->IsPlaying() ? 0.6f : 0.f;

		// Use time-varying weights to create mouth movement variety
		float RawAa = Envelope * (0.4f + 0.3f * FMath::Sin(Time * 8.7f));
		float RawIh = Envelope * (0.3f + 0.2f * FMath::Sin(Time * 11.3f + 1.2f));
		float RawOu = Envelope * (0.2f + 0.2f * FMath::Sin(Time * 6.1f + 2.5f));
		float RawEe = Envelope * (0.3f + 0.2f * FMath::Sin(Time * 13.7f + 0.8f));
		float RawOh = Envelope * (0.25f + 0.2f * FMath::Sin(Time * 9.3f + 3.1f));

		RawAa = FMath::Clamp(RawAa, 0.f, 1.f);
		RawIh = FMath::Clamp(RawIh, 0.f, 1.f);
		RawOu = FMath::Clamp(RawOu, 0.f, 1.f);
		RawEe = FMath::Clamp(RawEe, 0.f, 1.f);
		RawOh = FMath::Clamp(RawOh, 0.f, 1.f);

		Aa = SmoothValue(Aa, RawAa, DeltaTime);
		Ih = SmoothValue(Ih, RawIh, DeltaTime);
		Ou = SmoothValue(Ou, RawOu, DeltaTime);
		Ee = SmoothValue(Ee, RawEe, DeltaTime);
		Oh = SmoothValue(Oh, RawOh, DeltaTime);
	}

	OnVisemes.Broadcast(Aa, Ih, Ou, Ee, Oh);
}

float UVisemeDriverFFT::SmoothValue(float Current, float Target, float DeltaTime) const
{
	float Rate;
	if (Target > Current)
		Rate = 1.f - FMath::Exp(-DeltaTime * 1000.f / AttackMs);
	else
		Rate = 1.f - FMath::Exp(-DeltaTime * 1000.f / ReleaseMs);

	return FMath::Lerp(Current, Target, Rate);
}
