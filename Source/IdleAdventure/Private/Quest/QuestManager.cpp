


#include "Quest/QuestManager.h"
#include "PlayFab.h"
#include "Quest/Quest.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include "EngineUtils.h"
#include <Actor/NPCActor.h>
#include <Kismet/GameplayStatics.h>

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
    GetQuestData();
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
    Request.Keys.Add("WisdomQuest");
    Request.Keys.Add("TemperanceQuest");
    RequestedKeys.Add("WisdomQuest");
    RequestedKeys.Add("TemperanceQuest");

    clientAPI->GetTitleData(Request,
        PlayFab::UPlayFabClientAPI::FGetTitleDataDelegate::CreateUObject(this, &AQuestManager::OnGetQuestDataSuccess),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &AQuestManager::OnGetQuestDataFailure)
    );
}

void AQuestManager::OnGetQuestDataSuccess(const PlayFab::ClientModels::FGetTitleDataResult& Result)
{
    AvailableQuests.Empty();

    for (const FString& Key : RequestedKeys)
    {
        if (Result.Data.Contains(Key))
        {
            FString QuestDataJson = Result.Data[Key];
            TSharedPtr<FJsonObject> JsonObject;
            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(QuestDataJson);

            if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
            {
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
                NewQuest->QuestName = QuestName;
                NewQuest->QuestDescription = QuestDescription;
                NewQuest->Rewards = Rewards;

                AvailableQuests.Add(NewQuest);
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

    OnAddAvailableQuestsToUI.Broadcast(AvailableQuests);
    AssignQuestsToNPCs();
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