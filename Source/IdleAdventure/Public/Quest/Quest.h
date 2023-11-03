

#pragma once

#include "CoreMinimal.h"
#include "Quest/QuestEnums.h"
#include "UObject/NoExportTypes.h"
#include "Quest.generated.h"

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
	int32 Other;
};


USTRUCT(BlueprintType)
struct FQuestRewards
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Experience;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 TemperanceEssence;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FQuestObjectives Objectives;
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
	int32 Other;

	// Initialize all values to 0
	FQuestProgress() : Wisdom(0), Temperance(0), Justice(0), Courage(0), Other(0) {}

	// Method to update progress for a specific objective
	void UpdateProgress(const FString& ObjectiveType, int32 Amount)
	{
		if (ObjectiveType == "Wisdom") Wisdom += Amount;
		else if (ObjectiveType == "Temperance") Temperance += Amount;
		else if (ObjectiveType == "Justice") Justice += Amount;
		else if (ObjectiveType == "Courage") Courage += Amount;
		else if (ObjectiveType == "Other") Other += Amount;
	}

	// Check if the progress meets or exceeds the objectives
	bool IsComplete(const FQuestObjectives& Objectives) const
	{
		UE_LOG(LogTemp, Log, TEXT("Current Progress: Wisdom: %d, Temperance: %d, Justice: %d, Courage: %d, Other: %d"), Wisdom, Temperance, Justice, Courage, Other);
		UE_LOG(LogTemp, Log, TEXT("Required Objectives: Wisdom: %d, Temperance: %d, Justice: %d, Courage: %d, Other: %d"), Objectives.Wisdom, Objectives.Temperance, Objectives.Justice, Objectives.Courage, Objectives.Other);

		return Wisdom >= Objectives.Wisdom &&
			Temperance >= Objectives.Temperance &&
			Justice >= Objectives.Justice &&
			Courage >= Objectives.Courage &&
			Other >= Objectives.Other;
	}

	void ResetProgress(const FQuestObjectives& Objectives)
	{
		Wisdom = FMath::Max(0, Wisdom - Objectives.Wisdom);
		Temperance = FMath::Max(0, Temperance - Objectives.Temperance);
		Justice = FMath::Max(0, Justice - Objectives.Justice);
		Courage = FMath::Max(0, Courage - Objectives.Courage);
		Other = FMath::Max(0, Other - Objectives.Other);
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuestComplete);

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
	int32 Experience;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 TemperanceEssence;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FQuestRewards Rewards;

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestComplete OnQuestComplete;

	FQuestProgress QuestProgress;
};
