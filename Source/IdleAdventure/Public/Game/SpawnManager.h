

#pragma once

#include "CoreMinimal.h"
#include <Actor/IdleEffectActor.h>
#include "GameFramework/Actor.h"
#include "SpawnManager.generated.h"

USTRUCT(BlueprintType)
struct FRunCompleteRewards
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	FString Time;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	FString MapDifficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	float Experience;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	int32 Wisdom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	int32 Temperance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	int32 Justice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	int32 Courage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	int32 LegendaryEssence;

	// Constructor for initializing default values
	FRunCompleteRewards()
		: Time("")
		, MapDifficulty("")
		, Experience(0.0f)
		, Wisdom(0)
		, Temperance(0)
		, Justice(0)
		, Courage(0)
		, LegendaryEssence(0)
	{
	}
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRunCountsUpdated, float, Tree, float, Enemy, float, Boss);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRunComplete, FRunCompleteRewards, RunCompleteData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCountdownUpdated, const FString&, TimeRemaining);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnTimerAtZero, FString, Time, FString, Difficulty, FString, CauseOfDeath, FString, Tip);

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
	void UpdateBossCount(int32 Amount);
	void CheckRunComplete();
	void BroadcastInitialRunCounts();
	void UpdateCountdown();
	void BroadcastCountdownTime(float Time);
	void InitializeCountdown();
	FString GetFormattedTime(float TimeInSeconds);
	void SpawnTrees();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AIdleEffectActor> BlueprintToSpawn;

	int32 TreeCount;
	int32 EnemyCount;
	int32 BossCount;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCountdownUpdated OnCountdownUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRunCountsUpdated OnRunCountsUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRunComplete OnRunComplete;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTimerAtZero OnTimerAtZero;

	float CountdownTime;

	FRunCompleteRewards RewardsToSend;

private:
	static ASpawnManager* SpawnManagerSingletonInstance;
	FTimerHandle CountdownTimerHandle;
	
};
