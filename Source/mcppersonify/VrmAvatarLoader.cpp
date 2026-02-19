#include "VrmAvatarLoader.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

// VRM4U headers — these will only resolve when the plugin is installed.
// We use soft references so the project compiles without VRM4U,
// just with runtime loading disabled.
//
// When VRM4U is installed, uncomment these:
// #include "LoaderBPFunctionLibrary.h"
// #include "VrmAssetListObject.h"

UVrmAvatarLoader::UVrmAvatarLoader()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UVrmAvatarLoader::BeginPlay()
{
	Super::BeginPlay();

	if (!DefaultAvatarPath.IsEmpty())
	{
		LoadAvatar(DefaultAvatarPath);
	}
	else
	{
		// Auto-discover first .vrm in Content/Avatars
		FString AvatarDir = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Avatars"));
		TArray<FString> VrmFiles;
		IFileManager::Get().FindFiles(VrmFiles, *FPaths::Combine(AvatarDir, TEXT("*.vrm")), true, false);

		if (VrmFiles.Num() > 0)
		{
			FString FullPath = FPaths::Combine(AvatarDir, VrmFiles[0]);
			UE_LOG(LogTemp, Log, TEXT("[VrmAvatarLoader] Auto-loading: %s"), *VrmFiles[0]);
			LoadAvatar(FullPath);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[VrmAvatarLoader] No .vrm files in Content/Avatars/"));
		}
	}
}

void UVrmAvatarLoader::LoadAvatar(const FString& PathOrName)
{
	// Resolve path
	FString FullPath = PathOrName;
	if (!FPaths::FileExists(FullPath))
	{
		FullPath = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Avatars"), PathOrName);
	}

	if (!FPaths::FileExists(FullPath))
	{
		UE_LOG(LogTemp, Error, TEXT("[VrmAvatarLoader] VRM not found: %s"), *FullPath);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[VrmAvatarLoader] Loading: %s"), *FullPath);

	// Destroy previous
	UnloadAvatar();

	// ============================================================
	// VRM4U Runtime Loading
	// ============================================================
	// When VRM4U is installed and the headers are uncommented above,
	// replace this block with:
	//
	// UVrmAssetListObject* AssetList = ULoaderBPFunctionLibrary::LoadVRMFileLocal(
	//     this, FullPath, nullptr);
	//
	// if (AssetList)
	// {
	//     USkeletalMesh* Mesh = AssetList->GetSkeletalMesh();
	//     if (Mesh)
	//     {
	//         // Spawn actor with skeletal mesh component
	//         FActorSpawnParameters Params;
	//         Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//         CurrentAvatar = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), FTransform::Identity, Params);
	//
	//         USkeletalMeshComponent* MeshComp = NewObject<USkeletalMeshComponent>(CurrentAvatar);
	//         MeshComp->SetSkeletalMesh(Mesh);
	//         MeshComp->RegisterComponent();
	//         CurrentAvatar->SetRootComponent(MeshComp);
	//
	//         UE_LOG(LogTemp, Log, TEXT("[VrmAvatarLoader] Loaded: %s"), *Mesh->GetName());
	//         OnAvatarLoaded.Broadcast(CurrentAvatar);
	//     }
	// }
	// ============================================================

	// Placeholder: log that VRM4U is needed
	UE_LOG(LogTemp, Warning, TEXT("[VrmAvatarLoader] VRM4U plugin not yet integrated. "
		"Install VRM4U and uncomment the loading code in VrmAvatarLoader.cpp"));
	UE_LOG(LogTemp, Log, TEXT("[VrmAvatarLoader] File ready to load: %s"), *FullPath);
}

void UVrmAvatarLoader::UnloadAvatar()
{
	if (CurrentAvatar)
	{
		CurrentAvatar->Destroy();
		CurrentAvatar = nullptr;
	}
}
