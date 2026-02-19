#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VrmAvatarLoader.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAvatarLoaded, AActor*, AvatarActor);

/**
 * Loads VRM files at runtime using the VRM4U plugin.
 * Place VRM files in Content/Avatars/ or provide full paths.
 *
 * Requires VRM4U plugin installed in Plugins/VRM4U/.
 */
UCLASS(ClassGroup=(Personify), meta=(BlueprintSpawnableComponent))
class MCPPERSONIFY_API UVrmAvatarLoader : public UActorComponent
{
	GENERATED_BODY()

public:
	UVrmAvatarLoader();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Avatar")
	FString DefaultAvatarPath;

	UPROPERTY(BlueprintAssignable, Category="Avatar")
	FOnAvatarLoaded OnAvatarLoaded;

	UPROPERTY(BlueprintReadOnly, Category="Avatar")
	AActor* CurrentAvatar = nullptr;

	/**
	 * Load a VRM file. Provide a full path or just a filename
	 * (resolved relative to Content/Avatars/).
	 *
	 * NOTE: This uses VRM4U's LoadVRMFileLocal function.
	 * If VRM4U is not installed, it logs an error and returns.
	 */
	UFUNCTION(BlueprintCallable, Category="Avatar")
	void LoadAvatar(const FString& PathOrName);

	/** Destroy the currently loaded avatar. */
	UFUNCTION(BlueprintCallable, Category="Avatar")
	void UnloadAvatar();

protected:
	virtual void BeginPlay() override;
};
