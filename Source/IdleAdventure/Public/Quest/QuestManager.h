

#pragma once

#include "CoreMinimal.h"
#include <PlayFab.h>
#include "Quest/DialogueManager.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include "GameFramework/Actor.h"
#include "QuestManager.generated.h"


class UQuest;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAddAvailableQuestsToUI, UQuest*, QuestObject, FString, QuestName, FString, QuestDescription, FQuestRewards, Rewards);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddAvailableQuestsToUI, const TArray<UQuest*>&, Quests);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddCompletedQuestsToUI, const TArray<UQuest*>&, Quests);

UCLASS(Blueprintable)
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
    bool PlayerHasCompletedQuest(UQuest* Quest);
    void AssignQuestsToNPCs();
    void GivePlayerQuestRewards(UQuest* Quest);

    void BindToQuestDataReadyEvent();
    UFUNCTION()
    void HandleQuestDataReady();
    void CheckAllQuests();

    UFUNCTION(BlueprintCallable, Category = "Quests")
    void PlayerHasTooManyQuestsMessage();



    void GetCurrentEssenceCounts();
    void OnSuccessFetchEssenceCounts(const PlayFab::ClientModels::FGetUserDataResult& Result);
    void OnErrorFetchEssenceCounts(const PlayFab::FPlayFabCppError& Error);
    void SendUpdatedEssenceCountsToPlayFab();
    void OnSuccessUpdateEssenceCounts(const PlayFab::ClientModels::FUpdateUserDataResult& Result);
    void OnErrorUpdateEssenceCounts(const PlayFab::FPlayFabCppError& Error);


    //Main Story quest system
    //Hide them from regular quest log
    //UQuest* CreateOrUpdateLocalStoryQuest(FLocalQuest* LocalQuestData);

    //PlayFab stored
    int32 PlayFabWisdom;
    int32 PlayFabTemperance;
    int32 PlayFabJustice;
    int32 PlayFabCourage;
    int32 PlayFabLegendary;

    //Rewards from quest stored
    int32 WisdomReward = 0;
    int32 TemperanceReward = 0;
    int32 JusticeReward = 0;
    int32 CourageReward = 0;
    int32 LegendaryReward = 0;

    //FQuestProgress QuestProgress;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAddAvailableQuestsToUI OnAddAvailableQuestsToUI;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnAddCompletedQuestsToUI OnAddCompletedQuestsToUI;

    UPROPERTY(BlueprintReadOnly, Category = "Quests")
    TArray<UQuest*> AvailableQuests;

    UPROPERTY(BlueprintReadOnly, Category = "Quests")
    TArray<UQuest*> CompletedQuests;

    UPROPERTY(BlueprintReadOnly, Category = "Quests")
    TArray<UQuest*> AllLoadedQuests;

    UPROPERTY(BlueprintReadOnly, Category = "Quests")
    TArray<FString> RequestedKeys;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quests")
    int32 questCount;

    

private:
	static AQuestManager* QuestManagerSingletonInstance;
	PlayFabClientPtr clientAPI = nullptr;

};
