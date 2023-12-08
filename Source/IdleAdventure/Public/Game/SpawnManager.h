

#pragma once

#include "CoreMinimal.h"
#include <Actor/IdleEffectActor.h>
#include "GameFramework/Actor.h"
#include "SpawnManager.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRunCountsUpdated, float, Tree, float, Enemy);

UCLASS()
class IDLEADVENTURE_API ASpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawnManager();
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	static ASpawnManager* GetInstance(UWorld* World);
	void ResetInstance();

	int32 CountActorsWithTag(UWorld* World, const FName Tag);
	void UpdateTreeCount(int32 Amount);
	void UpdateEnemyCount(int32 Amount);
	void CheckRunComplete();
	void BroadcastInitialRunCounts();

	void SpawnTrees();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AIdleEffectActor> BlueprintToSpawn;

	int32 TreeCount;
	int32 EnemyCount;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRunCountsUpdated OnRunCountsUpdated;

private:
	static ASpawnManager* SpawnManagerSingletonInstance;
};
