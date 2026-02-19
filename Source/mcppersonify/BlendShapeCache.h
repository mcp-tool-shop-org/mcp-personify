#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"

/**
 * Scans a skeletal mesh's morph targets and provides fast name-based lookup.
 * Plain struct — not a UObject.
 */
struct FBlendShapeCache
{
	/** Build the cache from all SkeletalMeshComponents under a root. */
	void Build(AActor* Root);

	/** Check if a morph target exists (case-insensitive). */
	bool Has(const FString& Name) const;

	/** Set a morph target weight (0..1, case-insensitive). */
	void Set(const FString& Name, float Weight01);

	/** Get current morph target weight. */
	float Get(const FString& Name) const;

	/** Reset all morph targets to 0. */
	void ResetAll();

	/** Number of discovered morph targets. */
	int32 Count() const { return MorphMap.Num(); }

	/** All discovered morph target names (for diagnostics). */
	TArray<FString> GetAllNames() const;

private:
	struct FMorphEntry
	{
		USkeletalMeshComponent* MeshComp;
		FName MorphName;
	};

	/** Key = lowercase name -> morph entry */
	TMap<FString, FMorphEntry> MorphMap;
};
