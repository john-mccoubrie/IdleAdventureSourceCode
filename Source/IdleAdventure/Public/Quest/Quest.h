

#pragma once

#include "CoreMinimal.h"
#include "Quest/QuestEnums.h"
#include "UObject/NoExportTypes.h"
#include "Quest.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuestComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SevenParams(FUpdateQuestProgressDelegate, float, WisdomProgress, float, TemperanceProgress, float, JusticeProgress, 
	float, CourageProgress, float, LegendaryProgress, float, EnemyKillsProgress, float, BossKillsProgress);



USTRUCT(BlueprintType)
struct FQuestObjectives
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Wisdom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Temperance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Justice;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Courage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Legendary;

	// New properties for EnemyKills and BossKills
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 EnemyKills;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 BossKills;

	FQuestObjectives()
		: Wisdom(0)
		, Temperance(0)
		, Justice(0)
		, Courage(0)
		, Legendary(0)
		, EnemyKills(0)
		, BossKills(0)
	{
	}
};


USTRUCT(BlueprintType)
struct FQuestRewards
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Experience;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 WisdomEssence;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 TemperanceEssence;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 JusticeEssence;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 CourageEssence;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 LegendaryEssence;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FQuestObjectives Objectives;

	FQuestRewards()
		: Experience(0)
		, WisdomEssence(0)
		, TemperanceEssence(0)
		, JusticeEssence(0)
		, CourageEssence(0)
		, LegendaryEssence(0)
		, Objectives()
	{
	}
};

USTRUCT(BlueprintType)
struct FQuestProgress
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Wisdom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Temperance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Justice;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Courage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Legendary;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 EnemyKills;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 BossKills;

	// Initialize all values to 0
	FQuestProgress() : Wisdom(0), Temperance(0), Justice(0), Courage(0), Legendary(0) {}

	// Method to update progress for a specific objective
	void UpdateProgress(const FString& ObjectiveType, int32 Amount)
	{
		if (ObjectiveType == "Wisdom") Wisdom += Amount;
		else if (ObjectiveType == "Temperance") Temperance += Amount;
		else if (ObjectiveType == "Justice") Justice += Amount;
		else if (ObjectiveType == "Courage") Courage += Amount;
		else if (ObjectiveType == "Legendary") Legendary += Amount;
		else if (ObjectiveType == "EnemyKills") EnemyKills += Amount;
		else if (ObjectiveType == "BossKills") BossKills += Amount;
	}

	// Check if the progress meets or exceeds the objectives
	bool IsComplete(const FQuestObjectives& Objectives) const
	{
		UE_LOG(LogTemp, Log, TEXT("Current Progress: Wisdom: %d, Temperance: %d, Justice: %d, Courage: %d, Legendary: %d"), Wisdom, Temperance, Justice, Courage, Legendary);
		UE_LOG(LogTemp, Log, TEXT("Required Objectives: Wisdom: %d, Temperance: %d, Justice: %d, Courage: %d, Legendary: %d"), Objectives.Wisdom, Objectives.Temperance, Objectives.Justice, Objectives.Courage, Objectives.Legendary);

		return Wisdom >= Objectives.Wisdom &&
			Temperance >= Objectives.Temperance &&
			Justice >= Objectives.Justice &&
			Courage >= Objectives.Courage &&
			Legendary >= Objectives.Legendary;
			EnemyKills >= Objectives.EnemyKills &&
			BossKills >= Objectives.BossKills;
	}

	void ResetProgress(const FQuestObjectives& Objectives)
	{
		Wisdom = FMath::Max(0, Wisdom - Objectives.Wisdom);
		Temperance = FMath::Max(0, Temperance - Objectives.Temperance);
		Justice = FMath::Max(0, Justice - Objectives.Justice);
		Courage = FMath::Max(0, Courage - Objectives.Courage);
		Legendary = FMath::Max(0, Legendary - Objectives.Legendary);
		EnemyKills = FMath::Max(0, EnemyKills - Objectives.EnemyKills);
		BossKills = FMath::Max(0, BossKills - Objectives.BossKills);
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUpdateQuestTextValuesDelegate, FQuestProgress, Progress, FQuestObjectives, Objectives);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuestDetailsDelegate, FString, QuestName, int32, ExperienceReward);

UCLASS(BlueprintType)
class IDLEADVENTURE_API UQuest : public UObject
{
	GENERATED_BODY()
	

public:

	// Method to update progress
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void UpdateProgress(const FString& ObjectiveType, int32 Amount);

	// Method to check if the quest is complete
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool IsQuestComplete() const;

	void MarkQuestReadyToTurnIn();

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void TurnInQuest();

	void SetWorldContext(UWorld* InWorld);
	UWorld* GetWorldContext() const;
	UWorld* WorldContext;

	void SetInitialQuestValues();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest")
	EQuestState QuestState;

	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	bool bIsCompleted;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void Start();

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void Complete();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString QuestID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Version;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString QuestName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString QuestDescription;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString QuestCategory;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Quote;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsStarted;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Experience;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 WisdomEssence;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 TemperanceEssence;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 JusticeEssence;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 CourageEssence;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 LegendaryEssence;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 EnemyKills;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 BossKills;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FQuestRewards Rewards;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FQuestProgress QuestProgress;

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestComplete OnQuestComplete;

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FUpdateQuestProgressDelegate UpdateQuestProgressDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FUpdateQuestTextValuesDelegate UpdateQuestTextValuesDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FQuestDetailsDelegate QuestDetailsDelegate;
};
