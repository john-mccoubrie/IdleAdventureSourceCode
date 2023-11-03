

#pragma once

#include "CoreMinimal.h"
#include <PlayFab.h>
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include "GameFramework/Actor.h"
#include "QuestManager.generated.h"


class UQuest;

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

    void ResetProgress()
    {
        Wisdom = 0;
        Temperance = 0;
        Justice = 0;
        Courage = 0;
        Other = 0;
    }
};


/*
UENUM(BlueprintType)
enum class EQuestState : uint8
{
    NotStarted UMETA(DisplayName = "Not Started"),
    InProgress UMETA(DisplayName = "In Progress"),
    ReadyToTurnIn UMETA(DisplayName = "Ready to Turn In"),
    Completed UMETA(DisplayName = "Completed")
};
*/

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAddAvailableQuestsToUI, UQuest*, QuestObject, FString, QuestName, FString, QuestDescription, FQuestRewards, Rewards);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddAvailableQuestsToUI, const TArray<UQuest*>&, Quests);

UCLASS()
class IDLEADVENTURE_API AQuestManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQuestManager();
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	static AQuestManager* GetInstance(UWorld* World);
	void ResetInstance();
	void GetQuestData();
	void OnGetQuestDataSuccess(const PlayFab::ClientModels::FGetTitleDataResult& Result);
	void OnGetQuestDataFailure(const PlayFab::FPlayFabCppError& ErrorResult);
    void AssignQuestsToNPCs();

    FQuestProgress QuestProgress;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAddAvailableQuestsToUI OnAddAvailableQuestsToUI;

    UPROPERTY(BlueprintReadOnly, Category = "Quests")
    TArray<UQuest*> AvailableQuests;

    UPROPERTY(BlueprintReadOnly, Category = "Quests")
    TArray<FString> RequestedKeys;

private:
	static AQuestManager* QuestManagerSingletonInstance;
	PlayFabClientPtr clientAPI = nullptr;

};
