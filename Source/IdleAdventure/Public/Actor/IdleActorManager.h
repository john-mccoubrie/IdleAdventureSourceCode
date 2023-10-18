#pragma once

#include "CoreMinimal.h"
#include "IdleEffectActor.h"
#include "GameFramework/Actor.h"
#include "IdleActorManager.generated.h"

struct FTreeRespawnInfo
{
	FVector Location;
	FRotator Rotation;
	AIdleEffectActor* TreeActor;
};

UCLASS()
class IDLEADVENTURE_API AIdleActorManager : public AActor
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	void ResetInstance();
	// Function to spawn a tree at a specific location
	void SpawnTree(FVector Location, FRotator Rotation);
	void StartTreeCountdown(AIdleEffectActor* Tree, float TreeLifeSpan);
	void OnCountdownFinished(AIdleEffectActor* Tree);
	void ResetTreeTimer(AIdleEffectActor* Tree);
	void RespawnTree(AIdleEffectActor* Tree);
	FTimerHandle CountdownTimerHandle;

	// Function to handle tree cutting
	void CutTree(AIdleEffectActor* Tree);


	//Legendary Tree
	void GetLegendaryTree();
	void SelectNewLegendaryTree();
	void DeactivateCurrentLegendaryTree();
	TArray<AIdleEffectActor*> AllIdleEffectActors;
	AIdleEffectActor* LegendaryIdleEffectActor;
	

	static AIdleActorManager* GetInstance(UWorld* World);

	FVector RespawnLocation;
	FRotator RespawnRotation;
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