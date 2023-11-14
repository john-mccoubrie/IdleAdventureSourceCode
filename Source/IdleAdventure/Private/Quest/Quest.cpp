


#include "Quest/Quest.h"
#include "Quest/QuestManager.h"
#include <Player/IdlePlayerController.h>
#include <Player/IdlePlayerState.h>
#include <AbilitySystem/IdleAttributeSet.h>
#include <PlayFab/PlayFabManager.h>
#include <Chat/GameChatManager.h>

void UQuest::UpdateProgress(const FString& ObjectiveType, int32 Amount)
{

    QuestProgress.UpdateProgress(ObjectiveType, Amount);

    if (QuestProgress.IsComplete(Rewards.Objectives))
    {
        MarkQuestReadyToTurnIn();
    }


    float WisdomProgress = (Rewards.Objectives.Wisdom > 0) ? static_cast<float>(QuestProgress.Wisdom) / static_cast<float>(Rewards.Objectives.Wisdom) : 0.f;
    float TemperanceProgress = (Rewards.Objectives.Temperance > 0) ? static_cast<float>(QuestProgress.Temperance) / static_cast<float>(Rewards.Objectives.Temperance) : 0.f;
    float JusticeProgress = (Rewards.Objectives.Justice > 0) ? static_cast<float>(QuestProgress.Justice) / static_cast<float>(Rewards.Objectives.Justice) : 0.f;
    float CourageProgress = (Rewards.Objectives.Courage > 0) ? static_cast<float>(QuestProgress.Courage) / static_cast<float>(Rewards.Objectives.Courage) : 0.f;
    float OtherProgress = (Rewards.Objectives.Other > 0) ? static_cast<float>(QuestProgress.Other) / static_cast<float>(Rewards.Objectives.Other) : 0.f;

    UpdateQuestProgressDelegate.Broadcast(WisdomProgress, TemperanceProgress, JusticeProgress, CourageProgress, OtherProgress);
    //UpdateQuestTextValuesDelegate.Broadcast(QuestProgress.Wisdom, QuestProgress.Temperance, QuestProgress.Justice, QuestProgress.Courage, QuestProgress.Other,
        //Rewards.Objectives.Courage, Rewards.Objectives.Temperance, Rewards.Objectives.Justice, Rewards.Objectives.Courage, Rewards.Objectives.Other);

    UpdateQuestTextValuesDelegate.Broadcast(QuestProgress, Rewards.Objectives);

    QuestDetailsDelegate.Broadcast(QuestName, Rewards.Experience);
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
        AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorldContext());
        // Format the message using FString::Printf
        FString NotificationMessage = FString::Printf(TEXT("%s is ready to be turned in!"), *this->QuestName);
        // Convert FLinearColor to FSlateColor
        FSlateColor NotificationColor = FSlateColor(FLinearColor::Green);
        // Call the function with the formatted message and color
        GameChatManager->PostNotificationToUI(NotificationMessage, NotificationColor);
    }
}

void UQuest::TurnInQuest()
{
    //UE_LOG(LogTemp, Warning, TEXT("TurnInQuestCalled"));
    if (QuestState == EQuestState::ReadyToTurnIn)
    {
        QuestState = EQuestState::Completed;
        Complete();
        //UE_LOG(LogTemp, Error, TEXT("Quest turned in and completed!!!"));
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
    
    FTimerHandle TimerHandle;
    WorldContext->GetTimerManager().SetTimer(TimerHandle, this, &UQuest::SetInitialQuestValues, 1.0f, false);
}

void UQuest::SetInitialQuestValues()
{
    UE_LOG(LogTemp, Warning, TEXT("Broadcasted initial values"));
    //UpdateQuestProgressDelegate.Broadcast(WisdomProgress, TemperanceProgress, JusticeProgress, CourageProgress, OtherProgress);
    UpdateQuestTextValuesDelegate.Broadcast(QuestProgress, Rewards.Objectives);
    QuestDetailsDelegate.Broadcast(QuestName, Rewards.Experience);
}

void UQuest::Complete()
{
    //UE_LOG(LogTemp, Error, TEXT("Quest Complete!!!"));
    
    AQuestManager* QuestManager = AQuestManager::GetInstance(GetWorldContext());
    APlayFabManager* PlayFabManager = APlayFabManager::GetInstance(GetWorldContext());
    AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorldContext());

    // Format the message using FString::Printf
    FString NotificationMessage = FString::Printf(TEXT("You completed %s and got %i experience!"), *this->QuestName, this->Rewards.Experience);
    // Convert FLinearColor to FSlateColor
    FSlateColor NotificationColor = FSlateColor(FLinearColor::Green);
    // Call the function with the formatted message and color
    GameChatManager->PostNotificationToUI(NotificationMessage, NotificationColor);
   

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

