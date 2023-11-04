


#include "Quest/QuestManager.h"
#include "PlayFab.h"
#include "Quest/Quest.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include "EngineUtils.h"
#include <Actor/NPCActor.h>
#include <Kismet/GameplayStatics.h>
#include <Player/IdlePlayerController.h>
#include <Player/IdlePlayerState.h>
#include <AbilitySystem/IdleAttributeSet.h>
#include <PlayFab/PlayFabManager.h>

AQuestManager* AQuestManager::QuestManagerSingletonInstance = nullptr;

// Sets default values
AQuestManager::AQuestManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
    APlayFabManager* PlayFabManager = APlayFabManager::GetInstance(GetWorld());
    //PlayFabManager->OnQuestDataReady.AddDynamic(this, &AQuestManager::GetQuestData);
    PlayFabManager->OnQuestDataReady.AddDynamic(this, &AQuestManager::HandleQuestDataReady);;
    GetQuestData();
    BindToQuestDataReadyEvent();
}

void AQuestManager::BeginDestroy()
{
	Super::BeginDestroy();
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

                TSharedPtr<FJsonObject> RewardsObject = JsonObject->GetObjectField(TEXT("Rewards"));

                FQuestRewards Rewards;
                Rewards.Experience = RewardsObject->GetIntegerField(TEXT("Experience"));
                Rewards.TemperanceEssence = RewardsObject->GetIntegerField(TEXT("TemperanceEssence"));

                TSharedPtr<FJsonObject> ObjectivesObject = JsonObject->GetObjectField(TEXT("QuestObjectives"));
                FQuestObjectives Objectives;
                Objectives.Wisdom = ObjectivesObject->GetIntegerField(TEXT("Wisdom"));
                Objectives.Temperance = ObjectivesObject->GetIntegerField(TEXT("Temperance"));
                Objectives.Justice = ObjectivesObject->GetIntegerField(TEXT("Justice"));
                Objectives.Courage = ObjectivesObject->GetIntegerField(TEXT("Courage"));
                Objectives.Other = ObjectivesObject->GetIntegerField(TEXT("Other"));

                Rewards.Objectives = Objectives;

                UQuest* NewQuest = NewObject<UQuest>();
                NewQuest->QuestID = QuestID;
                NewQuest->Version = Version;
                NewQuest->QuestName = QuestName;
                NewQuest->QuestDescription = QuestDescription;
                NewQuest->Rewards = Rewards;
                AllLoadedQuests.Add(NewQuest);

                for (UQuest* Quest : AllLoadedQuests)
                {
                    
                     // Check if the quest has been completed using the data from PlayFab
                    if (PlayerHasCompletedQuest(Quest))
                    {
                        CompletedQuests.Add(Quest);
                        //OnAddCompletedQuestsToUI.Broadcast(CompletedQuests);
                        UE_LOG(LogTemp, Warning, TEXT("Added to Completed Quests: %s"), *Quest->QuestName);
                    }
                    else
                    {
                        AvailableQuests.Add(Quest);
                        //OnAddAvailableQuestsToUI.Broadcast(AvailableQuests);
                        UE_LOG(LogTemp, Warning, TEXT("Added to Available Quests: %s"), *Quest->QuestName);
                    }
                    
                }

                //AvailableQuests.Add(NewQuest);
                //UE_LOG(LogTemp, Error, TEXT("AddedQuest"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to parse quest data for key '%s' as JSON"), *Key);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Key '%s' not found in Result.Data"), *Key);
        }
    }

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
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), "NPC", FoundActors);
    for (UQuest* Quest : AvailableQuests)
    {
        for (AActor* Actor : FoundActors)
        {
            ANPCActor* NPCActor = Cast<ANPCActor>(Actor);
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
        // Bind a function to handle the event
        PlayFabManager->OnQuestDataReady.AddDynamic(this, &AQuestManager::HandleQuestDataReady);
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

void AQuestManager::GivePlayerQuestRewards(UQuest* Quest)
{
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);
    AQuestManager* QuestManager = AQuestManager::GetInstance(GetWorld());
    IdleAttributeSet->SetWoodcutExp(IdleAttributeSet->GetWoodcutExp() + Quest->Rewards.Experience);
}


/*


void AQuestManager::OnGetQuestDataSuccess(const PlayFab::ClientModels::FGetTitleDataResult& Result)
{
    if (Result.Data.Contains(TEXT("QuestTest")))
    {
        FString QuestDataJson = Result.Data[TEXT("QuestTest")];
        // Parse the JSON string to a JSON object
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(QuestDataJson);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            // Now extract the data from the JSON object
            FString QuestName = JsonObject->GetStringField(TEXT("Name"));
            FString QuestDescription = JsonObject->GetStringField(TEXT("Description"));

            TSharedPtr<FJsonObject> RewardsObject = JsonObject->GetObjectField(TEXT("Rewards"));

            FQuestRewards Rewards;
            Rewards.Experience = RewardsObject->GetIntegerField(TEXT("Experience"));
            Rewards.TemperanceEssence = RewardsObject->GetIntegerField(TEXT("TemperanceEssence"));


            TSharedPtr<FJsonObject> ObjectivesObject = JsonObject->GetObjectField(TEXT("QuestObjectives"));
            FQuestObjectives Objectives;
            Objectives.Wisdom = ObjectivesObject->GetIntegerField(TEXT("Wisdom"));
            Objectives.Temperance = ObjectivesObject->GetIntegerField(TEXT("Temperance"));
            Objectives.Justice = ObjectivesObject->GetIntegerField(TEXT("Justice"));
            Objectives.Courage = ObjectivesObject->GetIntegerField(TEXT("Courage"));
            Objectives.Other = ObjectivesObject->GetIntegerField(TEXT("Other"));

            Rewards.Objectives = Objectives;


            // Create the quest object
            UQuest* NewQuest = NewObject<UQuest>();
            NewQuest->QuestName = QuestName;
            NewQuest->QuestDescription = QuestDescription;
            NewQuest->Rewards = Rewards; // Assign the struct here

            TArray<AActor*> FoundActors;
            UGameplayStatics::GetAllActorsWithTag(GetWorld(), "NPC", FoundActors);
            ANPCActor* NPCActor = FoundActors.Num() > 0 ? Cast<ANPCActor>(FoundActors[0]) : nullptr;

            OnAddAvailableQuestsToUI.Broadcast(NewQuest, NewQuest->QuestName, NewQuest->QuestDescription, NewQuest->Rewards);

            if (NPCActor != nullptr)
            {
                NPCActor->AddAvailableQuests(NewQuest);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("No NPC Actor found with the specified tag."));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to parse QuestTest data as JSON"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestTest key not found in Result.Data"));
    }
}


*/