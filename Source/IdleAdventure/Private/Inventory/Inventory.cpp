


#include "Inventory/Inventory.h"
#include "Character/IdleCharacter.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include <Kismet/GameplayStatics.h>
#include <Chat/GameChatManager.h>
#include <Player/IdlePlayerController.h>


// Sets default values
AInventory::AInventory()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
}

void AInventory::BeginPlay()
{
    Super::BeginPlay();

    //InitializeEssenceCounts();
    //Delay the PlayFab data pull by 3 seconds to allow level to load
    FTimerHandle PlayFabPullDelayTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(PlayFabPullDelayTimerHandle, this, &AInventory::RequestPlayFabData, 3.0f, false);
}

void AInventory::AddItem(UItem* Item)
{
    if (Item != nullptr)
    {
        // Ensure all essence types are initialized to 0 if they don't already exist in the map
        TArray<FName> AllEssenceTypes = {
            FName(TEXT("Wisdom")),
            FName(TEXT("Temperance")),
            FName(TEXT("Justice")),
            FName(TEXT("Courage")),
            FName(TEXT("Legendary"))
        };
        for (const FName& EssenceType : AllEssenceTypes)
        {
            EssenceCount.FindOrAdd(EssenceType);
        }

        EssenceCountForDurationCalc++;

        Items.Add(Item);
        boolOnItemAdded.Broadcast(true);
        OnItemAdded.Broadcast(Item->EssenceRarity);
        CheckInventorySize();
        AIdleCharacter* Character = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        Character->EssenceCount++;
        EssenceCount.FindOrAdd(Item->EssenceRarity) += 1;
        for (const auto& EssenceEntry : EssenceCount)
        {
            //UE_LOG(LogTemp, Warning, TEXT("Essence: %s, Count: %d"), *EssenceEntry.Key.ToString(), EssenceEntry.Value);
        }
        //UE_LOG(LogTemp, Warning, TEXT("Item Added in Inventory class"));
    }
}

void AInventory::RemoveItem(UItem* Item)
{
    Items.Remove(Item);
    boolOnItemAdded.Broadcast(false);
    OnItemRemoved.Broadcast(Item->EssenceRarity);
    EssenceCountForDurationCalc = 0;

    TransferAndClearEssenceCounts();
    /*
    int32* EssenceCountPtr = EssenceCount.Find(Item->EssenceRarity);
    if (EssenceCountPtr && *EssenceCountPtr > 1)
    {
        // If the count is greater than 1, decrement it
        (*EssenceCountPtr)--;
    }
    else if (EssenceCountPtr && *EssenceCountPtr == 1)
    {
        // If the count is 1, remove the key-value pair from the map
        EssenceCount.Remove(Item->EssenceRarity);
    }
    */
}

void AInventory::CheckInventorySize()
{
    AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
    AIdleCharacter* Character = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (Character->EssenceCount == 24)
    {
        AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
        //PC->InterruptTreeCutting();
        GameChatManager->PostNotificationToUI(TEXT("Inventory is full! Add your essence to the nearest coffer. Inventory"));

        //TODO:
        //Stop wooductting animation, tree timers, etc.
    }
}

void AInventory::TransferAndClearEssenceCounts()
{
    // Transfer the essence counts
    for (const auto& EssenceEntry : EssenceCount)
    {
        EssenceAddedToCoffer.FindOrAdd(EssenceEntry.Key) += EssenceEntry.Value;
        //UE_LOG(LogTemp, Warning, TEXT("Essence: %s, Count: %d"), *EssenceEntry.Key.ToString(), EssenceEntry.Value);
    }

    TArray<FEssenceCoffer> EssenceCofferArray;
    for (const auto& KeyValue : EssenceAddedToCoffer)
    {
        FEssenceCoffer EssenceCoffer;
        EssenceCoffer.Key = KeyValue.Key;
        EssenceCoffer.Value = KeyValue.Value;
        EssenceCofferArray.Add(EssenceCoffer);
    }

    OnEssenceTransferred.Broadcast(EssenceCofferArray);

    // Serialize the updated EssenceAddedToCoffer map to a JSON object
    TSharedPtr<FJsonObject> JsonObject = TMapToJsonObject(EssenceAddedToCoffer);
    FString UpdatedDataString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&UpdatedDataString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    //UE_LOG(LogTemp, Log, TEXT("Updating PlayFab with essence data: %s"), *UpdatedDataString);

    // Create a request to update the PlayFab user data
    PlayFab::ClientModels::FUpdateUserDataRequest UpdateRequest;
    UpdateRequest.Data.Add(TEXT("EssenceAddedToCoffer"), UpdatedDataString);
    clientAPI->UpdateUserData(UpdateRequest,
        PlayFab::UPlayFabClientAPI::FUpdateUserDataDelegate::CreateUObject(this, &AInventory::OnUpdateDataSuccess),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &AInventory::OnUpdateDataFailure)
    );
    

    // Format the message with the counts of each essence type from the current transfer
    FString formattedMessage = FString::Printf(
        TEXT("You added %d Wisdom essence, %d Temperance essence, %d Justice essence, %d Courage essence, and %d Legendary essence to the coffer"),
        EssenceCount.FindOrAdd(FName(TEXT("Wisdom"))),
        EssenceCount.FindOrAdd(FName(TEXT("Temperance"))),
        EssenceCount.FindOrAdd(FName(TEXT("Justice"))),
        EssenceCount.FindOrAdd(FName(TEXT("Courage"))),
        EssenceCount.FindOrAdd(FName(TEXT("Legendary")))
    );

    // Send the formatted message to the game chat manager
    AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
    GameChatManager->PostNotificationToUI(formattedMessage);


    EssenceCount.Empty();

    for (const auto& EssenceEntry : EssenceAddedToCoffer)
    {
        //UE_LOG(LogTemp, Warning, TEXT("Total Essence: %s, Count: %d"), *EssenceEntry.Key.ToString(), EssenceEntry.Value);
    }
}

void AInventory::OnGetDataSuccess(const PlayFab::ClientModels::FGetUserDataResult& Result)
{
    auto FoundData = Result.Data.Find(TEXT("EssenceAddedToCoffer"));
    if (FoundData)
    {
        FString ReceivedString = FoundData->Value;
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ReceivedString);
        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            EssenceAddedToCoffer = JsonObjectToTMap(JsonObject);  // Assume JsonObjectToTMap is your deserialization function
        }
    }

    // Create an array of FEssenceCoffer to be broadcasted
    TArray<FEssenceCoffer> EssenceCofferArray;
    for (const auto& KeyValue : EssenceAddedToCoffer)
    {
        FEssenceCoffer EssenceCoffer;
        EssenceCoffer.Key = KeyValue.Key;
        EssenceCoffer.Value = KeyValue.Value;
        EssenceCofferArray.Add(EssenceCoffer);
    }

    // Broadcast the OnEssenceTransferred event
    OnEssenceTransferred.Broadcast(EssenceCofferArray);
}

void AInventory::OnGetDataFailure(const PlayFab::FPlayFabCppError& Error)
{
    UE_LOG(LogTemp, Error, TEXT("Failed to get user data from PlayFab: %s"), *Error.GenerateErrorReport());
}

void AInventory::OnUpdateDataSuccess(const PlayFab::ClientModels::FUpdateUserDataResult& Result)
{
    //UE_LOG(LogTemp, Warning, TEXT("Successfully updated user data on PlayFab."));

    /*
    // Create an array of FEssenceCoffer to be broadcasted
    TArray<FEssenceCoffer> EssenceCofferArray;
    for (const auto& KeyValue : EssenceAddedToCoffer)
    {
        FEssenceCoffer EssenceCoffer;
        EssenceCoffer.Key = KeyValue.Key;
        EssenceCoffer.Value = KeyValue.Value;
        EssenceCofferArray.Add(EssenceCoffer);
    }

    // Broadcast the OnEssenceTransferred event
    OnEssenceTransferred.Broadcast(EssenceCofferArray);

    */
}

void AInventory::OnUpdateDataFailure(const PlayFab::FPlayFabCppError& Error)
{
    UE_LOG(LogTemp, Error, TEXT("Failed to update user data on PlayFab: %s"), *Error.GenerateErrorReport());
}

void AInventory::RequestPlayFabData()
{
    PlayFab::ClientModels::FGetUserDataRequest GetRequest;
    clientAPI->GetUserData(GetRequest,
        PlayFab::UPlayFabClientAPI::FGetUserDataDelegate::CreateUObject(this, &AInventory::OnGetDataSuccess),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &AInventory::OnGetDataFailure)
    );
}

void AInventory::InitializeEssenceCounts()
{
    //Initializes all essence counts to zero before the update so none are missed (and for new players)
    TArray<FName> AllEssenceTypes = {
        FName(TEXT("Wisdom")),
        FName(TEXT("Temperance")),
        FName(TEXT("Justice")),
        FName(TEXT("Courage")),
        FName(TEXT("Legendary"))
    };

    for (const FName& EssenceType : AllEssenceTypes)
    {
        EssenceCount.FindOrAdd(EssenceType) = 0;
    }
}

TSharedPtr<FJsonObject> AInventory::TMapToJsonObject(const TMap<FName, int32>& Map)
{
    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    for (const auto& KeyValue : Map)
    {
        JsonObject->SetNumberField(KeyValue.Key.ToString(), KeyValue.Value);
    }
    return JsonObject;
}

TMap<FName, int32> AInventory::JsonObjectToTMap(const TSharedPtr<FJsonObject>& JsonObject)
{
    TMap<FName, int32> Map;
    for (const auto& KeyValue : JsonObject->Values)
    {
        FName Key(*KeyValue.Key);
        int32 Value = KeyValue.Value->AsNumber();
        Map.Add(Key, Value);
    }
    return Map;
}


int32 AInventory::GetEssenceCount(FName EssenceType) const
{
    return EssenceCount.Contains(EssenceType) ? EssenceCount[EssenceType] : 0;
}

TMap<FName, int32> AInventory::GetAllEssenceCounts() const
{
    return EssenceCount;
}



