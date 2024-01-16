


#include "Quest/Quest.h"
#include "Quest/QuestManager.h"
#include <Player/IdlePlayerController.h>
#include <Player/IdlePlayerState.h>
#include <AbilitySystem/IdleAttributeSet.h>
#include <PlayFab/PlayFabManager.h>
#include <Chat/GameChatManager.h>
#include <Save/IdleSaveGame.h>
#include <Kismet/GameplayStatics.h>
#include <Game/SteamManager.h>

void UQuest::UpdateProgress(const FString& ObjectiveType, int32 Amount)
{
    UE_LOG(LogTemp, Warning, TEXT("Update quest progress"));
    QuestProgress.UpdateProgress(ObjectiveType, Amount);

    if (QuestProgress.IsComplete(Rewards.Objectives))
    {
        MarkQuestReadyToTurnIn();
    }


    float WisdomProgress = (Rewards.Objectives.Wisdom > 0) ? static_cast<float>(QuestProgress.Wisdom) / static_cast<float>(Rewards.Objectives.Wisdom) : 0.f;
    float TemperanceProgress = (Rewards.Objectives.Temperance > 0) ? static_cast<float>(QuestProgress.Temperance) / static_cast<float>(Rewards.Objectives.Temperance) : 0.f;
    float JusticeProgress = (Rewards.Objectives.Justice > 0) ? static_cast<float>(QuestProgress.Justice) / static_cast<float>(Rewards.Objectives.Justice) : 0.f;
    float CourageProgress = (Rewards.Objectives.Courage > 0) ? static_cast<float>(QuestProgress.Courage) / static_cast<float>(Rewards.Objectives.Courage) : 0.f;
    float LegendaryProgress = (Rewards.Objectives.Legendary > 0) ? static_cast<float>(QuestProgress.Legendary) / static_cast<float>(Rewards.Objectives.Legendary) : 0.f;
    float EnemyKillsProgress = (Rewards.Objectives.EnemyKills > 0) ? static_cast<float>(QuestProgress.EnemyKills) / static_cast<float>(Rewards.Objectives.EnemyKills) : 0.f;
    float BossKillsProgress = (Rewards.Objectives.BossKills > 0) ? static_cast<float>(QuestProgress.BossKills) / static_cast<float>(Rewards.Objectives.BossKills) : 0.f;

    UpdateQuestProgressDelegate.Broadcast(WisdomProgress, TemperanceProgress, JusticeProgress, CourageProgress, LegendaryProgress, EnemyKillsProgress, BossKillsProgress);
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
        //UE_LOG(LogTemp, Warning, TEXT("Quest is ready to be turned in."));
        AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorldContext());
        // Format the message using FString::Printf
        FString NotificationMessage = FString::Printf(TEXT("%s is ready to be turned in!"), *this->QuestName);

        // Define the light sky blue color
        FLinearColor LightSkyBlue = FLinearColor(135.0f / 255.0f, 206.0f / 255.0f, 250.0f / 255.0f, 1.0f);
        // Convert FLinearColor to FSlateColor
        FSlateColor NotificationColor = FSlateColor(LightSkyBlue);
        // Call the function with the formatted message and color
        GameChatManager->PostNotificationToUI(NotificationMessage, NotificationColor);

        AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorldContext()->GetFirstPlayerController());
        PC->IdleInteractionComponent->PlayQuestReadyForTurnInSound();
    }
}

void UQuest::TurnInQuest()
{
    //UE_LOG(LogTemp, Warning, TEXT("TurnInQuestCalled"));
    if (QuestState == EQuestState::ReadyToTurnIn)
    {
        QuestState = EQuestState::Completed;
        
        //check if the tutorial is complete and the quest has a quote to post
        if (this->QuestCategory == "Wisdom" || this->QuestCategory == "Temperance" || this->QuestCategory == "Justice" || this->QuestCategory == "Courage")
        {
            UIdleSaveGame* LoadGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("TutorialSaveSlot", 0));
            if (LoadGameInstance && LoadGameInstance->IsTutorialCompleted())
            {
                AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorldContext());
                GameChatManager->PostQuoteToMeditationsJournal(this->Quote, this->QuestCategory);
            }
            else
            {
                AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorldContext());
                GameChatManager->PostQuoteToMeditationsJournal(this->Quote, this->QuestCategory);
                UE_LOG(LogTemp, Warning, TEXT("Tutorial not completed, but still posting quote to meditations"));
            }
        }
        
        Complete();
        //UE_LOG(LogTemp, Error, TEXT("Quest turned in and completed!!!"));
        // Reset the progress values
        //AQuestManager* QuestManager = AQuestManager::GetInstance(GetWorld());
        //QuestManager->QuestProgress.ResetProgress();
        //QuestProgress.ResetProgress();
        QuestProgress.ResetProgress(Rewards.Objectives);

        AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorldContext()->GetFirstPlayerController());
        PC->IdleInteractionComponent->PlayQuestTurnInSound();
    }
    else
    {
        //FString QuestToTurnIn = this->QuestName;
        AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorldContext());
        GameChatManager->PostNotificationToUI(TEXT("this quest is not ready to be turned in yet!"),FLinearColor::Red);
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
    AQuestManager* QuestManager = AQuestManager::GetInstance(GetWorldContext());
    QuestManager->questCount++;
    FTimerHandle TimerHandle;
    if (WorldContext)
    {
        WorldContext->GetTimerManager().SetTimer(TimerHandle, this, &UQuest::SetInitialQuestValues, 1.0f, false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No world context in UQuest!"));
    }
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
    QuestManager->questCount--;
    // Format the message using FString::Printf
    FString NotificationMessage = FString::Printf(TEXT("You completed %s and got %i experience, %i wisdom essence, %i temperance essence, %i justice essence, %i courage essence, and %i legendary essence!"), *this->QuestName, this->Rewards.Experience, this->Rewards.WisdomEssence, this->Rewards.TemperanceEssence, this->Rewards.JusticeEssence, this->Rewards.CourageEssence, this->Rewards.LegendaryEssence);
    // Define the light sky blue color
    FLinearColor LightSkyBlue = FLinearColor(135.0f / 255.0f, 206.0f / 255.0f, 250.0f / 255.0f, 1.0f);
    // Convert FLinearColor to FSlateColor
    FSlateColor NotificationColor = FSlateColor(LightSkyBlue);
    // Call the function with the formatted message and color
    GameChatManager->PostNotificationToUI(NotificationMessage, NotificationColor);


    if (this->QuestName == "TutorialComplete")
    {
        // Try to load an existing save game
        UIdleSaveGame* SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("TutorialSaveSlot", 0));

        // If it doesn't exist, create a new instance
        if (!SaveGameInstance)
        {
            SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::CreateSaveGameObject(UIdleSaveGame::StaticClass()));
        }

        // Set the tutorial as completed
        SaveGameInstance->SetTutorialCompleted(true); // Assuming bIsTutorialCompleted is marked with UPROPERTY(SaveGame)

        // Save the game to the same slot to store the updated data
        UGameplayStatics::SaveGameToSlot(SaveGameInstance, "TutorialSaveSlot", 0);

        // Log completion and broadcast events
        UE_LOG(LogTemp, Warning, TEXT("Completed quest for: %s"), *this->QuestName);
        QuestManager->OnTutorialComplete.Broadcast();
        GameChatManager->PostNotificationToUI(TEXT("You have completed the tutorial and gained access to Meditations, complete activities in the world to unlock more quotes!"), FLinearColor::Yellow);

        //Unlock tutorial achievement
        ASteamManager* SteamManager = ASteamManager::GetInstance(GetWorld());
        if (SteamManager)
        {
            SteamManager->UnlockSteamAchievement(TEXT("COMPLETE_TUTORIAL"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Quest name is not TutorialComplete, it is: %s"), *this->QuestName);
    }

   

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

