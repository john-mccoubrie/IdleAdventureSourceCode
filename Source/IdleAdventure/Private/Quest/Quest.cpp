


#include "Quest/Quest.h"
#include "Quest/QuestManager.h"
#include <Player/IdlePlayerController.h>
#include <Player/IdlePlayerState.h>
#include <AbilitySystem/IdleAttributeSet.h>
#include <PlayFab/PlayFabManager.h>

void UQuest::UpdateProgress(const FString& ObjectiveType, int32 Amount)
{

    QuestProgress.UpdateProgress(ObjectiveType, Amount);

    if (QuestProgress.IsComplete(Rewards.Objectives))
    {
        MarkQuestReadyToTurnIn();
    }

    /*
    UE_LOG(LogTemp, Warning, TEXT("Made it to update progress in Quest"));
    AQuestManager* QuestManager = AQuestManager::GetInstance(GetWorld());
    QuestManager->QuestProgress.UpdateProgress(ObjectiveType, Amount);
    // Optionally, check if the quest is completed after each update
    if (QuestManager->QuestProgress.IsComplete(Rewards.Objectives))
    {
        UE_LOG(LogTemp, Warning, TEXT("Made it inside questiscomplete loop"));
        MarkQuestReadyToTurnIn(); // Instead of completing, mark it as ready to turn in
    }
    */
}

bool UQuest::IsQuestComplete() const
{
    //AQuestManager* QuestManager = AQuestManager::GetInstance(GetWorld());
    //return QuestManager->QuestProgress.IsComplete(Rewards.Objectives);
    return QuestProgress.IsComplete(Rewards.Objectives);
}

void UQuest::MarkQuestReadyToTurnIn()
{
    if (QuestState == EQuestState::InProgress && IsQuestComplete())
    {
        QuestState = EQuestState::ReadyToTurnIn;
        UE_LOG(LogTemp, Warning, TEXT("Quest is ready to be turned in."));
    }
}

void UQuest::TurnInQuest()
{
    UE_LOG(LogTemp, Warning, TEXT("TurnInQuestCalled"));
    if (QuestState == EQuestState::ReadyToTurnIn)
    {
        Complete();
        QuestState = EQuestState::Completed;
        UE_LOG(LogTemp, Error, TEXT("Quest turned in and completed!!!"));
        // Reset the progress values
        //AQuestManager* QuestManager = AQuestManager::GetInstance(GetWorld());
        //QuestManager->QuestProgress.ResetProgress();
        //QuestProgress.ResetProgress();
        QuestProgress.ResetProgress(Rewards.Objectives);
    }
}

void UQuest::SetWorldContext(UWorld* InWorld)
{
    WorldContext = InWorld;
}

UWorld* UQuest::GetWorldContext() const
{
    return WorldContext;
}

void UQuest::Start()
{
    // Logic to start the quest, for example setting initial objectives or flags
    QuestState = EQuestState::InProgress;
    UE_LOG(LogTemp, Warning, TEXT("QuestStarted"));
}

void UQuest::Complete()
{
    UE_LOG(LogTemp, Error, TEXT("Quest Complete!!!"));
    
    AQuestManager* QuestManager = AQuestManager::GetInstance(GetWorldContext());
    
    

    APlayFabManager* PlayFabManager = APlayFabManager::GetInstance(GetWorldContext());
   

    if (QuestManager && PlayFabManager)
    {
        PlayFabManager->CompleteQuest(this);
        QuestManager->GivePlayerQuestRewards(this);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("playfab manager or quest manager not valid in quest"));
    }

    OnQuestComplete.Broadcast();
}

