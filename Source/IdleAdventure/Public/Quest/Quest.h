

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
};
