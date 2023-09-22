#pragma once

#include "CoreMinimal.h"
#include "IdleEffectActor.h"
#include "GameFramework/Actor.h"
#include "IdleActorManager.generated.h"

struct FTreeRespawnInfo
{
	FVector Location;
	FRotator Rotation;
};

UCLASS()
class IDLEADVENTURE_API AIdleActorManager : public AActor
{
	GENERATED_BODY()

public:

	virtual void BeginDestroy() override;
	void ResetInstance();
	// Function to spawn a tree at a specific location
	void SpawnTree(FVector Location, FRotator Rotation);

	// Function to handle tree cutting
	void CutTree(AIdleEffectActor* Tree);

	// Function to respawn a tree after it's been cut down
	UFUNCTION()
	void RespawnTree(FName TreeName); // Adjusted the parameter type

	static AIdleActorManager* GetInstance(UWorld* World);

	FVector RespawnLocation;
	FRotator RespawnRotation;
	AIdleEffectActor* TreeToRespawn;
	UPROPERTY(EditAnywhere, Category = "Actors")
	TArray<AIdleEffectActor*> CutTrees;

	// Adjusted the key type to FName for the maps
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	TMap<FName, FTreeRespawnInfo> TreeRespawnMap;
	TMap<FName, bool> TreeChoppingStates;
	TMap<FName, FTimerHandle> TreeTimers;

	FTimerHandle ClickedTreeTimerHandle;

private:
	// Timer handle for respawning trees
	FTimerHandle RespawnTreeTimerHandle;

	static AIdleActorManager* Instance;

	int32 TreeCounterName = 0;
};