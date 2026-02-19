#include "BlendShapeCache.h"

void FBlendShapeCache::Build(AActor* Root)
{
	MorphMap.Empty();

	if (!Root) return;

	TArray<USkeletalMeshComponent*> MeshComps;
	Root->GetComponents<USkeletalMeshComponent>(MeshComps);

	for (auto* MeshComp : MeshComps)
	{
		USkeletalMesh* Mesh = MeshComp->GetSkeletalMeshAsset();
		if (!Mesh) continue;

		const TArray<UMorphTarget*>& MorphTargets = Mesh->GetMorphTargets();
		for (auto* Morph : MorphTargets)
		{
			if (!Morph) continue;
			FString Key = Morph->GetName().ToLower();
			if (!MorphMap.Contains(Key))
			{
				MorphMap.Add(Key, { MeshComp, Morph->GetFName() });
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[BlendShapeCache] Found %d morph targets across %d mesh components"),
		MorphMap.Num(), MeshComps.Num());
}

bool FBlendShapeCache::Has(const FString& Name) const
{
	return MorphMap.Contains(Name.ToLower());
}

void FBlendShapeCache::Set(const FString& Name, float Weight01)
{
	const FString Key = Name.ToLower();
	if (const FMorphEntry* Entry = MorphMap.Find(Key))
	{
		Entry->MeshComp->SetMorphTarget(Entry->MorphName, FMath::Clamp(Weight01, 0.f, 1.f));
	}
}

float FBlendShapeCache::Get(const FString& Name) const
{
	const FString Key = Name.ToLower();
	if (const FMorphEntry* Entry = MorphMap.Find(Key))
	{
		return Entry->MeshComp->GetMorphTarget(Entry->MorphName);
	}
	return 0.f;
}

void FBlendShapeCache::ResetAll()
{
	for (auto& Pair : MorphMap)
	{
		Pair.Value.MeshComp->SetMorphTarget(Pair.Value.MorphName, 0.f);
	}
}

TArray<FString> FBlendShapeCache::GetAllNames() const
{
	TArray<FString> Names;
	for (auto& Pair : MorphMap)
	{
		Names.Add(Pair.Value.MorphName.ToString());
	}
	return Names;
}
