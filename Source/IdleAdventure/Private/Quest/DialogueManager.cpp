


#include "Quest/DialogueManager.h"
#include "EngineUtils.h"
#include "Quest/QuestManager.h"
#include <Kismet/GameplayStatics.h>
#include <Character/IdleCharacter.h>
#include <Save/IdleSaveGame.h>
#include <Chat/GameChatManager.h>

ADialogueManager* ADialogueManager::DialogueManagerSingletonInstnace = nullptr;

// Sets default values
ADialogueManager::ADialogueManager()
{
    DialogueDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Blueprints/DataTables/DT_Dialogue.DT_Dialogue")));
    LocalQuestDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Blueprints/DataTables/DT_LocalQuestData.DT_LocalQuestData")));
}

// Called when the game starts or when spawned
void ADialogueManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (!DialogueManagerSingletonInstnace)
	{
		DialogueManagerSingletonInstnace = this;
	}

    //For testing purposes
    UIdleSaveGame* SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::CreateSaveGameObject(UIdleSaveGame::StaticClass()));
    SaveGameInstance->DialogueStep = 0;
    UGameplayStatics::SaveGameToSlot(SaveGameInstance, "DialogueStepSaveSlot", 0);
}

void ADialogueManager::BeginDestroy()
{
	Super::BeginDestroy();

	ResetInstance();
}

ADialogueManager* ADialogueManager::GetInstance(UWorld* World)
{
    if (!DialogueManagerSingletonInstnace)
    {
        for (TActorIterator<ADialogueManager> It(World); It; ++It)
        {
            DialogueManagerSingletonInstnace = *It;
            break;
        }
        if (!DialogueManagerSingletonInstnace)
        {
            DialogueManagerSingletonInstnace = World->SpawnActor<ADialogueManager>();
        }
    }
    return DialogueManagerSingletonInstnace;
}

void ADialogueManager::ResetInstance()
{
	DialogueManagerSingletonInstnace = nullptr;
}

int32 ADialogueManager::GetCurrentDialogue()
{
    UIdleSaveGame* LoadGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("DialogueStepSaveSlot", 0));
    if (LoadGameInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("Get current dialogue step: %i"), LoadGameInstance->DialogueStep);
        return LoadGameInstance->DialogueStep;
    }
    return 0;
}

void ADialogueManager::SaveDialogueStep(int32 DialogueToSave)
{
    UIdleSaveGame* SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::CreateSaveGameObject(UIdleSaveGame::StaticClass()));
    SaveGameInstance->DialogueStep = DialogueToSave;
    UE_LOG(LogTemp, Warning, TEXT("Save dialogue step: %i"), SaveGameInstance->DialogueStep);
    UGameplayStatics::SaveGameToSlot(SaveGameInstance, "DialogueStepSaveSlot", 0);
}

bool ADialogueManager::IsDialogueQuestStart(int32 DialogueStep)
{
    return false;
}

UQuest* ADialogueManager::GetQuestObjectivesForStory(const FString& QuestName)
{
    if (LocalQuestDataTable != nullptr && !QuestName.IsEmpty())
    {
        FLocalQuest* LocalQuestData = LocalQuestDataTable->FindRow<FLocalQuest>(FName(*QuestName), TEXT(""));
        if (LocalQuestData) // Check if the pointer is not null
        {
            UQuest* NewQuest = CreateOrUpdateLocalStoryQuest(*LocalQuestData);
            if (NewQuest)
            {
                UE_LOG(LogTemp, Warning, TEXT("Quest data found for: %s"), *QuestName);
                GetMainStoryQuest.Broadcast(NewQuest);
                NewQuest->SetWorldContext(GetWorld());
                NewQuest->Start();
                APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
                AIdleCharacter* Character = Cast<AIdleCharacter>(PlayerController->GetPawn());
                //Character->AddQuest(NewQuest);
                Character->ActiveQuests.Add(NewQuest);
                return NewQuest;
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Quest data not found for QuestName: %s"), *QuestName);
            return nullptr;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid QuestName or LocalQuestDataTable is nullptr."));
        return nullptr;
    }
    return nullptr;
}

UQuest* ADialogueManager::CreateOrUpdateLocalStoryQuest(const FLocalQuest& LocalQuestData)
{
    UQuest* Quest = NewObject<UQuest>();

    // Set or update quest properties
    Quest->QuestID = LocalQuestData.QuestID;
    Quest->QuestName = LocalQuestData.Name;
    Quest->QuestDescription = LocalQuestData.Description;
    Quest->Rewards.Objectives = LocalQuestData.Objectives;
    Quest->Rewards = LocalQuestData.Rewards;
    Quest->QuestProgress = LocalQuestData.Progress;


    AQuestManager* QuestManager = AQuestManager::GetInstance(GetWorld());
    QuestManager->AvailableQuests.Add(Quest);
    // Start or update the quest based on the dialogue
    //Quest->StartOrUpdateQuestFromDialogue();

    return Quest;
}

bool ADialogueManager::IsMainStoryQuestReadyToTurnIn(UQuest* QuestToCheck)
{
    if (QuestToCheck->QuestState == EQuestState::ReadyToTurnIn)
    {
        return true;
    }
    AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
    GameChatManager->PostNotificationToUI(TEXT("Quest is not complete yet!"), FLinearColor::Red);
    return false;
}

void ADialogueManager::CompleteMainStoryQuest(UQuest* QuestToComplete)
{
    AQuestManager* QuestManager = AQuestManager::GetInstance(GetWorld());
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    AIdleCharacter* Character = Cast<AIdleCharacter>(PlayerController->GetPawn());

    QuestManager->AvailableQuests.Remove(QuestToComplete);
    Character->ActiveQuests.Remove(QuestToComplete);
    QuestToComplete->Complete();
}

void ADialogueManager::SaveMainStoryQuestProgress(FQuestProgress ProgressToSave, UQuest* QuestToSave)
{
    UIdleSaveGame* SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::CreateSaveGameObject(UIdleSaveGame::StaticClass()));

    // Assuming UIdleSaveGame has a member variable to store FQuestProgress
    //SaveGameInstance->SavedQuestProgress = ProgressToSave;

    if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, QuestToSave->QuestID, 0))
    {
        UE_LOG(LogTemp, Warning, TEXT("Quest progress saved successfully for Quest ID: %s"), *QuestToSave->QuestID);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to save quest progress for Quest ID: %s"), *QuestToSave->QuestID);
    }
}

void ADialogueManager::UpdateAndSaveQuestProgress(UQuest* Quest, const FString& ObjectiveType, int32 Amount)
{
    if (!Quest)
    {
        UE_LOG(LogTemp, Warning, TEXT("Quest pointer is null."));
        return;
    }
    // Load existing progress
    FQuestProgress ExistingProgress = LoadMainStoryQuestProgress(Quest->QuestID);

    // Update the specified essence type's progress
    if (ObjectiveType == "Wisdom") ExistingProgress.Wisdom += Amount;
    else if (ObjectiveType == "Temperance") ExistingProgress.Temperance += Amount;
    else if (ObjectiveType == "Justice") ExistingProgress.Justice += Amount;
    else if (ObjectiveType == "Courage") ExistingProgress.Courage += Amount;
    else if (ObjectiveType == "Other") ExistingProgress.Legendary += Amount;
    else if (ObjectiveType == "EnemyKills") ExistingProgress.EnemyKills += Amount;
    else if (ObjectiveType == "BossKills") ExistingProgress.BossKills += Amount;
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid ObjectiveType: %s"), *ObjectiveType);
        return;
    }
    //might only need to do this on load
    //bool isGameFirstLoad;
    if (isGameFirstLoad)
    {
        Quest->SetWorldContext(GetWorld());
        Quest->UpdateProgress("Wisdom", ExistingProgress.Wisdom);
        Quest->UpdateProgress("Temperance", ExistingProgress.Temperance);
        Quest->UpdateProgress("Justice", ExistingProgress.Justice);
        Quest->UpdateProgress("Courage", ExistingProgress.Courage);
        Quest->UpdateProgress("EnemyKills", ExistingProgress.EnemyKills);
        Quest->UpdateProgress("BossKills", ExistingProgress.BossKills);
        isGameFirstLoad = false;
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Updated progress for %s: %d"), *ObjectiveType, Amount);

    // Save the updated progress
    SaveMainStoryQuestProgress(ExistingProgress, Quest);

    if (ObjectiveType == "Wisdom") ExistingProgress.Wisdom -= Amount;
    else if (ObjectiveType == "Temperance") ExistingProgress.Temperance -= Amount;
    else if (ObjectiveType == "Justice") ExistingProgress.Justice -= Amount;
    else if (ObjectiveType == "Courage") ExistingProgress.Courage -= Amount;
    else if (ObjectiveType == "Other") ExistingProgress.Legendary -= Amount;
    else if (ObjectiveType == "EnemyKills") ExistingProgress.EnemyKills -= Amount;
    else if (ObjectiveType == "BossKills") ExistingProgress.BossKills -= Amount;
}


FQuestProgress ADialogueManager::LoadMainStoryQuestProgress(FString QuestID)
{

    // Check if the save game exists
    if (UGameplayStatics::DoesSaveGameExist(QuestID, 0))
    {
        // Load the saved game data
        UIdleSaveGame* LoadedGame = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot(QuestID, 0));
        if (LoadedGame)
        {
            // Access the saved quest progress
            //FQuestProgress LoadedQuestProgress = LoadedGame->SavedQuestProgress;
            //OnLoadMainStoryQuestProgress.Broadcast(LoadedQuestProgress);
            //return LoadedQuestProgress;
            //Reset progress for testing
            return GetDefaultProgress();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to load game data for Quest ID: %s"), *QuestID);
            return GetDefaultProgress();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No saved game data found for Quest ID: %s , getting default progress"), *QuestID);
        return GetDefaultProgress();
    }
}

FQuestProgress ADialogueManager::GetDefaultProgress()
{
    FQuestProgress DefaultProgress;
    DefaultProgress.Wisdom = 0;
    DefaultProgress.Temperance = 0;
    DefaultProgress.Justice = 0;
    DefaultProgress.Courage = 0;
    DefaultProgress.Legendary = 0;
    DefaultProgress.EnemyKills = 0;
    DefaultProgress.BossKills = 0;
    return DefaultProgress;
}

bool ADialogueManager::isQuestCompleteOnLoad(UQuest* QuestToCheck)
{
    UpdateAndSaveQuestProgress(QuestToCheck, "Wisdom", 0);
    //QuestToCheck->SetWorldContext(GetWorld());
   // QuestToCheck->UpdateProgress("Wisdom", 0);
    AQuestManager* QuestManager = AQuestManager::GetInstance(GetWorld());
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    AIdleCharacter* Character = Cast<AIdleCharacter>(PlayerController->GetPawn());
    QuestToCheck->Start();
    QuestManager->AvailableQuests.Add(QuestToCheck);
    Character->ActiveQuests.Add(QuestToCheck);
    bool bIsComplete = QuestToCheck->IsQuestComplete();
    if (bIsComplete)
    {
        UE_LOG(LogTemp, Warning, TEXT("Quest is complete!"));
        //int32 UpdateDialogueStep = GetCurrentDialogue();
        //UpdateDialogueStep += 2;
        //SaveDialogueStep(UpdateDialogueStep);
        //GetCurrentDialogue();
        QuestManager->AvailableQuests.Remove(QuestToCheck);
        Character->ActiveQuests.Remove(QuestToCheck);
        QuestToCheck->Complete();
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Quest is NOT complete!"));
        return false;
    }
}


