


#include "Quest/QuestManager.h"
#include "PlayFab.h"
#include "Quest/Quest.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include "EngineUtils.h"
#include <Actor/NPC_QuestGiver.h>
#include <Kismet/GameplayStatics.h>
#include <Player/IdlePlayerController.h>
#include <Player/IdlePlayerState.h>
#include <AbilitySystem/IdleAttributeSet.h>
#include <PlayFab/PlayFabManager.h>
#include <Chat/GameChatManager.h>

AQuestManager* AQuestManager::QuestManagerSingletonInstance = nullptr;

// Sets default values
AQuestManager::AQuestManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    questCount = 0;
}

// Called when the game starts or when spawned
void AQuestManager::BeginPlay()
{
	Super::BeginPlay();
	if (!QuestManagerSingletonInstance)
	{
		QuestManagerSingletonInstance = this;
	}
	clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
    //APlayFabManager* PlayFabManager = APlayFabManager::GetInstance(GetWorld());
    //PlayFabManager->OnQuestDataReady.AddDynamic(this, &AQuestManager::GetQuestData);
    //PlayFabManager->OnQuestDataReady.AddDynamic(this, &AQuestManager::HandleQuestDataReady);;
    //GetQuestData();
    //BindToQuestDataReadyEvent();
}

void AQuestManager::BeginDestroy()
{
	Super::BeginDestroy();

    APlayFabManager* PlayFabManager = APlayFabManager::GetInstance(GetWorld());
    if (PlayFabManager)
    {
        PlayFabManager->OnQuestDataReady.RemoveDynamic(this, &AQuestManager::HandleQuestDataReady);
    }

	ResetInstance();
}

AQuestManager* AQuestManager::GetInstance(UWorld* World)
{
    if (!QuestManagerSingletonInstance)
    {
        for (TActorIterator<AQuestManager> It(World); It; ++It)
        {
            QuestManagerSingletonInstance = *It;
            break;
        }
        if (!QuestManagerSingletonInstance)
        {
            QuestManagerSingletonInstance = World->SpawnActor<AQuestManager>();
        }
    }
    return QuestManagerSingletonInstance;
}

void AQuestManager::ResetInstance()
{
    QuestManagerSingletonInstance = nullptr;
}

void AQuestManager::GetQuestData()
{
    UE_LOG(LogTemp, Warning, TEXT("GetQuestData called"));
    PlayFab::ClientModels::FGetTitleDataRequest Request;
    Request.Keys.Add("DailyEasyQuest");
    Request.Keys.Add("DailyLegendaryQuest");
    Request.Keys.Add("TheDailyGrind");
    Request.Keys.Add("TheDailyStoic");
    RequestedKeys.Add("DailyEasyQuest");
    RequestedKeys.Add("DailyLegendaryQuest");
    RequestedKeys.Add("TheDailyGrind");
    RequestedKeys.Add("TheDailyStoic");

    clientAPI->GetTitleData(Request,
        PlayFab::UPlayFabClientAPI::FGetTitleDataDelegate::CreateUObject(this, &AQuestManager::OnGetQuestDataSuccess),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &AQuestManager::OnGetQuestDataFailure)
    );
}

void AQuestManager::OnGetQuestDataSuccess(const PlayFab::ClientModels::FGetTitleDataResult& Result)
{
    AvailableQuests.Empty();
    CompletedQuests.Empty();
    AllLoadedQuests.Empty();

    for (const FString& Key : RequestedKeys)
    {
        if (Result.Data.Contains(Key))
        {
            FString QuestDataJson = Result.Data[Key];
            TSharedPtr<FJsonObject> JsonObject;
            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(QuestDataJson);

            if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
            {
                FString QuestID = JsonObject->GetStringField(TEXT("QuestID"));
                FString Version = JsonObject->GetStringField(TEXT("Version"));
                FString QuestName = JsonObject->GetStringField(TEXT("Name"));
                FString QuestDescription = JsonObject->GetStringField(TEXT("Description"));
                FString QuestCategory = JsonObject->GetStringField(TEXT("Category"));

                TSharedPtr<FJsonObject> RewardsObject = JsonObject->GetObjectField(TEXT("Rewards"));

                FQuestRewards Rewards;
                Rewards.Experience = RewardsObject->GetIntegerField(TEXT("Experience"));
                Rewards.WisdomEssence = RewardsObject->GetIntegerField(TEXT("WisdomEssence"));
                Rewards.TemperanceEssence = RewardsObject->GetIntegerField(TEXT("TemperanceEssence"));
                Rewards.JusticeEssence = RewardsObject->GetIntegerField(TEXT("JusticeEssence"));
                Rewards.CourageEssence = RewardsObject->GetIntegerField(TEXT("CourageEssence"));
                Rewards.LegendaryEssence = RewardsObject->GetIntegerField(TEXT("LegendaryEssence"));

                TSharedPtr<FJsonObject> ObjectivesObject = JsonObject->GetObjectField(TEXT("QuestObjectives"));
                FQuestObjectives Objectives;
                Objectives.Wisdom = ObjectivesObject->GetIntegerField(TEXT("Wisdom"));
                Objectives.Temperance = ObjectivesObject->GetIntegerField(TEXT("Temperance"));
                Objectives.Justice = ObjectivesObject->GetIntegerField(TEXT("Justice"));
                Objectives.Courage = ObjectivesObject->GetIntegerField(TEXT("Courage"));
                Objectives.Other = ObjectivesObject->GetIntegerField(TEXT("Other"));

                Rewards.Objectives = Objectives;

                UQuest* NewQuest = NewObject<UQuest>();
                NewQuest->SetWorldContext(GetWorld());
                NewQuest->QuestID = QuestID;
                NewQuest->Version = Version;
                NewQuest->QuestName = QuestName;
                NewQuest->QuestDescription = QuestDescription;
                NewQuest->QuestCategory = QuestCategory;
                NewQuest->Rewards = Rewards;
                AllLoadedQuests.Add(NewQuest);
            }
        }
                
    }
            // After all quests have been loaded, determine if they are completed or available
            for (UQuest* Quest : AllLoadedQuests)
            {
                if (PlayerHasCompletedQuest(Quest) && !CompletedQuests.Contains(Quest))
                {
                    CompletedQuests.Add(Quest);
                    UE_LOG(LogTemp, Warning, TEXT("Added to Completed Quests: %s"), *Quest->QuestName);
                }
                else
                {
                    if (!AvailableQuests.Contains(Quest))
                    {
                        AvailableQuests.Add(Quest);
                        UE_LOG(LogTemp, Warning, TEXT("Added to Available Quests: %s"), *Quest->QuestName);
                    } 
                }
            }
            OnAddCompletedQuestsToUI.Broadcast(CompletedQuests);
            OnAddAvailableQuestsToUI.Broadcast(AvailableQuests);
            AssignQuestsToNPCs();
}

bool AQuestManager::PlayerHasCompletedQuest(UQuest* Quest)
{
    FString* LastCompletedDatePtr = nullptr;
    APlayFabManager* PlayFabManager = APlayFabManager::GetInstance(GetWorld());
    if (PlayFabManager)
    {
        // Find returns a pointer to the value if the key exists, otherwise nullptr.
        LastCompletedDatePtr = PlayFabManager->PlayerCompletedQuestsData.Find(Quest->QuestID);

        // If the pointer is not null, that means the key exists in the map
        if (LastCompletedDatePtr)
        {
            // Dereference the pointer to get the actual FString value
            FString LastCompletedDate = *LastCompletedDatePtr;
            return !PlayFabManager->NeedsReset(LastCompletedDate);
            questCount++;
        }
    }
    return false; // Quest not found in completed quests data, or PlayFabManager instance not found
}

void AQuestManager::OnGetQuestDataFailure(const PlayFab::FPlayFabCppError& ErrorResult)
{
    UE_LOG(LogTemp, Error, TEXT("Failed to get quest data: %s"), *ErrorResult.ErrorMessage);
}

void AQuestManager::AssignQuestsToNPCs()
{
    UE_LOG(LogTemp, Warning, TEXT("AssignQuestsToNPC called"));
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), "NPC", FoundActors);
    for (UQuest* Quest : AvailableQuests)
    {
        for (AActor* Actor : FoundActors)
        {
            ANPC_QuestGiver* NPCActor = Cast<ANPC_QuestGiver>(Actor);
            if (NPCActor != nullptr)
            {
                NPCActor->AddAvailableQuests(Quest);
            }
        }
    }
}

void AQuestManager::BindToQuestDataReadyEvent()
{
    APlayFabManager* PlayFabManager = APlayFabManager::GetInstance(GetWorld());
    if (PlayFabManager)
    {
        //PlayFabManager->OnQuestDataReady.AddDynamic(this, &AQuestManager::HandleQuestDataReady);  
    }
}

void AQuestManager::HandleQuestDataReady()
{
    // Data is ready, now it's safe to check quests
    CheckAllQuests();


    AssignQuestsToNPCs();

    OnAddCompletedQuestsToUI.Broadcast(CompletedQuests); // If you need to broadcast the whole list at once
    OnAddAvailableQuestsToUI.Broadcast(AvailableQuests);
}

void AQuestManager::CheckAllQuests()
{
    // Assume this function iterates over quests and checks their completion
    // based on the now available completed quests data
    for (UQuest* Quest : AllLoadedQuests)
    {
        if (PlayerHasCompletedQuest(Quest))
        {
            CompletedQuests.Add(Quest);
        }
        else
        {
            AvailableQuests.Add(Quest);
        }
    }
}

void AQuestManager::PlayerHasTooManyQuestsMessage()
{
    AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
    GameChatManager->PostNotificationToUI(TEXT("You may only have 2 active quests at one time!"), FLinearColor::Red);
}

void AQuestManager::GetCurrentEssenceCounts()
{
    PlayFab::ClientModels::FGetUserDataRequest GetRequest;
    clientAPI->GetUserData(GetRequest,
        PlayFab::UPlayFabClientAPI::FGetUserDataDelegate::CreateUObject(this, &AQuestManager::OnSuccessFetchEssenceCounts),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &AQuestManager::OnErrorFetchEssenceCounts));
}

void AQuestManager::OnSuccessFetchEssenceCounts(const PlayFab::ClientModels::FGetUserDataResult& Result)
{
    FString EssenceDataJsonString;

    // Assuming "EssenceAddedToCoffer" is the key for the essence counts data
    if (Result.Data.Contains(TEXT("EssenceAddedToCoffer")))
    {
        EssenceDataJsonString = Result.Data[TEXT("EssenceAddedToCoffer")].Value;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Essence counts data not found."));
        return;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(EssenceDataJsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        // Parse and assign the values to your int32 variables
        PlayFabWisdom = JsonObject->GetIntegerField(TEXT("Wisdom"));
        PlayFabJustice = JsonObject->GetIntegerField(TEXT("Justice"));
        PlayFabTemperance = JsonObject->GetIntegerField(TEXT("Temperance"));
        PlayFabCourage = JsonObject->GetIntegerField(TEXT("Courage"));
        PlayFabLegendary = JsonObject->GetIntegerField(TEXT("Legendary"));

        // Log or use these values as needed
        UE_LOG(LogTemp, Log, TEXT("Wisdom: %d, Justice: %d, Temperance: %d, Courage: %d, Legendary: %d"), PlayFabWisdom, PlayFabJustice, PlayFabTemperance, PlayFabCourage, PlayFabLegendary);

        // Update any relevant game data structures or variables here
        PlayFabWisdom += WisdomReward;
        PlayFabTemperance += TemperanceReward;
        PlayFabJustice += JusticeReward;
        PlayFabCourage += CourageReward;
        PlayFabLegendary += LegendaryReward;

        SendUpdatedEssenceCountsToPlayFab();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse essence counts JSON."));
    }
}

void AQuestManager::OnErrorFetchEssenceCounts(const PlayFab::FPlayFabCppError& Error)
{
    FString ErrorMessage = Error.GenerateErrorReport();
    UE_LOG(LogTemp, Error, TEXT("Failed to get essence counts in NPC_Investor: %s"), *ErrorMessage);
}

void AQuestManager::SendUpdatedEssenceCountsToPlayFab()
{
    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    JsonObject->SetNumberField(TEXT("Wisdom"), PlayFabWisdom);
    JsonObject->SetNumberField(TEXT("Temperance"), PlayFabTemperance);
    JsonObject->SetNumberField(TEXT("Justice"), PlayFabJustice);
    JsonObject->SetNumberField(TEXT("Courage"), PlayFabCourage);
    JsonObject->SetNumberField(TEXT("Legendary"), PlayFabLegendary);

    FString UpdatedDataString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&UpdatedDataString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    PlayFab::ClientModels::FUpdateUserDataRequest UpdateRequest;
    UpdateRequest.Data.Add(TEXT("EssenceAddedToCoffer"), UpdatedDataString);

    clientAPI->UpdateUserData(UpdateRequest,
        PlayFab::UPlayFabClientAPI::FUpdateUserDataDelegate::CreateUObject(this, &AQuestManager::OnSuccessUpdateEssenceCounts),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &AQuestManager::OnErrorUpdateEssenceCounts));
}

void AQuestManager::OnSuccessUpdateEssenceCounts(const PlayFab::ClientModels::FUpdateUserDataResult& Result)
{
    UE_LOG(LogTemp, Log, TEXT("Successfully updated essence counts on PlayFab."));
    //Broadcast to UI
    APlayFabManager* PlayFabManager = APlayFabManager::GetInstance(GetWorld());
    PlayFabManager->OnEssenceUpdate.Broadcast(PlayFabWisdom, PlayFabTemperance, PlayFabJustice, PlayFabCourage, PlayFabLegendary);
}

void AQuestManager::OnErrorUpdateEssenceCounts(const PlayFab::FPlayFabCppError& Error)
{
    FString ErrorMessage = Error.GenerateErrorReport();
    UE_LOG(LogTemp, Error, TEXT("Failed to update essence counts on PlayFab: %s"), *ErrorMessage);
}

void AQuestManager::GivePlayerQuestRewards(UQuest* Quest)
{
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);
    AQuestManager* QuestManager = AQuestManager::GetInstance(GetWorld());
    IdleAttributeSet->SetWoodcutExp(IdleAttributeSet->GetWoodcutExp() + Quest->Rewards.Experience);
    //give the player justice essence reward and save it to playfab
    GetCurrentEssenceCounts();
    
    WisdomReward = Quest->Rewards.WisdomEssence;
    TemperanceReward = Quest->Rewards.TemperanceEssence;
    JusticeReward = Quest->Rewards.JusticeEssence;
    CourageReward = Quest->Rewards.CourageEssence;
    LegendaryReward = Quest->Rewards.LegendaryEssence;
}
