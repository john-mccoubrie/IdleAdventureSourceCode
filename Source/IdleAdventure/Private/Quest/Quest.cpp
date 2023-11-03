


#include "Quest/Quest.h"
#include "Quest/QuestManager.h"

void UQuest::UpdateProgress(const FString& ObjectiveType, int32 Amount)
{
    UE_LOG(LogTemp, Warning, TEXT("Made it to update progress in Quest"));
    AQuestManager* QuestManager = AQuestManager::GetInstance(GetWorld());
    QuestManager->QuestProgress.UpdateProgress(ObjectiveType, Amount);
    // Optionally, check if the quest is completed after each update
    if (QuestManager->QuestProgress.IsComplete(Rewards.Objectives))
    {
        UE_LOG(LogTemp, Warning, TEXT("Made it inside questiscomplete loop"));
        MarkQuestReadyToTurnIn(); // Instead of completing, mark it as ready to turn in
    }
}

bool UQuest::IsQuestComplete() const
{
    AQuestManager* QuestManager = AQuestManager::GetInstance(GetWorld());
    return QuestManager->QuestProgress.IsComplete(Rewards.Objectives);
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
        Complete(); // Call the Complete method if the quest is ready to be turned in
        QuestState = EQuestState::Completed;
        UE_LOG(LogTemp, Error, TEXT("Quest turned in and completed!!!"));
        // Reset the progress values
        AQuestManager* QuestManager = AQuestManager::GetInstance(GetWorld());
        QuestManager->QuestProgress.ResetProgress();
    }
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
    OnQuestComplete.Broadcast();
}

