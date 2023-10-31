


#include "PlayFab/PlayFabManager.h"
#include <Kismet/GameplayStatics.h>
#include "EngineUtils.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include <PlayerEquipment/PlayerEquipment.h>
#include <Chat/GameChatManager.h>

//Define the static member variable
APlayFabManager* APlayFabManager::SingletonInstance = nullptr;

APlayFabManager::APlayFabManager()
{
    //UE_LOG(LogTemp, Warning, TEXT("PlayFabManager constructor called"));
    
}

void APlayFabManager::BeginPlay()
{
    Super::BeginPlay();

    clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
    // Ensure the singleton instance is set on BeginPlay
    if (!SingletonInstance)
    {
        SingletonInstance = this;
    }
    Character = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    LoadPlayerEquipmentInventory();
}

void APlayFabManager::BeginDestroy()
{
    Super::BeginDestroy();
    //UE_LOG(LogTemp, Warning, TEXT("AIdleActorManager is being destroyed"));
    ResetInstance();
}

APlayFabManager* APlayFabManager::GetInstance(UWorld* World)
{
    if (!SingletonInstance)
    {
        for (TActorIterator<APlayFabManager> It(World); It; ++It)
        {
            SingletonInstance = *It;
            break;
        }
        if (!SingletonInstance)
        {
            SingletonInstance = World->SpawnActor<APlayFabManager>();
        }
    }
    return SingletonInstance;
}

void APlayFabManager::ResetInstance()
{
    SingletonInstance = nullptr;
}


bool APlayFabManager::PurchaseEquipment(const FString& EquipmentName, const FEquipmentData& EquipmentData)
{

    if (DataTableRowNames.Contains(FName(*EquipmentName)))
    {
        // Item already exists, do not proceed with purchase
        AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
        FString formattedMessage = FString::Printf(TEXT("You already purchased the %s"), *EquipmentName);
        GameChatManager->PostNotificationToUI(formattedMessage);
        UE_LOG(LogTemp, Warning, TEXT("in playfab manager, Item already exists in inventory: %s"), *EquipmentName);
        return false;
    }
    // Create an array to hold the new equipment data
    TArray<FEquipmentData> NewPlayerEquipmentInventory;
    NewPlayerEquipmentInventory.Add(EquipmentData);
    return true;
    /*
    // Update the player equipment inventory on PlayFab
    if (UpdatePlayerEquipmentInventory(NewPlayerEquipmentInventory))
    {
        return true;
    }
    else
    {
        return false;
    }
    */

    // Update local inventory immediately upon successful purchase
    //DataTableRowNames.Add(FName(*EquipmentName));
}

bool APlayFabManager::UpdatePlayFabEssenceCount(const FEquipmentData& EquipmentData)
{
    SuccessfulUpdateCount = 0;
    // Array of essence types
    TArray<FName> EssenceTypes = {
        FName(TEXT("Wisdom")),
        FName(TEXT("Temperance")),
        FName(TEXT("Justice")),
        FName(TEXT("Courage")),
        FName(TEXT("Legendary"))
    };

    // Array of corresponding costs from EquipmentData
    TArray<int32> Costs = {
        EquipmentData.WisdomCost,
        EquipmentData.TemperanceCost,
        EquipmentData.JusticeCost,
        EquipmentData.CourageCost,
        EquipmentData.LegendaryCost
    };

    if (!Character || !Character->CharacterInventory)
    {
        UE_LOG(LogTemp, Warning, TEXT("Character or inventory are null ptr in PlayFabManager"));
        return false;
    }

    // Ensure all essence types have enough essence to cover the costs
    for (int32 i = 0; i < EssenceTypes.Num(); ++i)
    {
        //AIdleCharacter* Character = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        
            int32* CurrentEssenceCount = Character->CharacterInventory->EssenceAddedToCoffer.Find(EssenceTypes[i]);
            if (CurrentEssenceCount && *CurrentEssenceCount < Costs[i])
            {
                UE_LOG(LogTemp, Warning, TEXT("Insufficient essence of type: %s"), *EssenceTypes[i].ToString());
                return false;
            }
        
    }

    // Deduct the costs and update on PlayFab
    for (int32 i = 0; i < EssenceTypes.Num(); ++i)
    {
        //AIdleCharacter* Character = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        
            int32* CurrentEssenceCount = Character->CharacterInventory->EssenceAddedToCoffer.Find(EssenceTypes[i]);
            if (CurrentEssenceCount)
            {
                if (*CurrentEssenceCount >= Costs[i])
                {
                    *CurrentEssenceCount -= Costs[i];
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Insufficient essence of type: %s"), *EssenceTypes[i].ToString());
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Essence type not found: %s"), *EssenceTypes[i].ToString());
            }

            // Now update the essence count on PlayFab
            // Assume UpdatePlayFabEssenceCount is a method that handles updating essence count on PlayFab
            if (CurrentEssenceCount)
            {
                if (!TryPlayFabUpdate(EssenceTypes[i], *CurrentEssenceCount))
                {
                    // Add the essence cost back since the update on PlayFab failed
                    *CurrentEssenceCount += Costs[i];
                    UE_LOG(LogTemp, Error, TEXT("Failed to update essence count of type: %s on PlayFab"), *EssenceTypes[i].ToString());

                    // Revert the other essence types
                    for (int32 j = 0; j < i; ++j)
                    {
                        int32* RevertEssenceCount = Character->CharacterInventory->EssenceAddedToCoffer.Find(EssenceTypes[i]);
                        *RevertEssenceCount += Costs[j];
                        // Optionally: revert the count on PlayFab as well
                    }
                    return false;
                }
            }
            
        
    }

    // Update EssenceAddedToCoffer on PlayFab after updating essence counts
    UpdateEssenceAddedToCofferOnPlayFab();

    return true;
    
}

bool APlayFabManager::UpdatePlayerEquipmentInventory(const TArray<FEquipmentData>& NewPlayerEquipmentInventory)
{
    // Convert TArray to a format suitable for PlayFab
    // Assume ConvertToPlayFabFormat is a method that converts TArray to a PlayFab compatible format
    auto PlayFabInventoryData = ConvertToPlayFabFormat(NewPlayerEquipmentInventory);

    // Create a request to update the PlayFab user data
    PlayFab::ClientModels::FUpdateUserDataRequest UpdateRequest;
    UpdateRequest.Data.Add(TEXT("PlayerEquipmentInventory"), PlayFabInventoryData);

    // Send the request to PlayFab
    clientAPI->UpdateUserData(UpdateRequest,
        PlayFab::UPlayFabClientAPI::FUpdateUserDataDelegate::CreateUObject(this, &APlayFabManager::OnSuccessUpdateInventory),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &APlayFabManager::OnErrorUpdateInventory));

    return true;
}

void APlayFabManager::LoadPlayerEquipmentInventory()
{
    if (!clientAPI.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("clientAPI is not valid in LoadPlayerEquipmentInventory"));
        return;
    }

    // Create a request to get the user data from PlayFab
    PlayFab::ClientModels::FGetUserDataRequest GetRequest;

    // Send the request to PlayFab
    clientAPI->GetUserData(GetRequest,
        PlayFab::UPlayFabClientAPI::FGetUserDataDelegate::CreateUObject(this, &APlayFabManager::OnSuccessFetchInventory),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &APlayFabManager::OnErrorFetchInventory));
}

void APlayFabManager::OnSuccessFetchInventory(const PlayFab::ClientModels::FGetUserDataResult& Result)
{
    //UE_LOG(LogTemp, Warning, TEXT("OnSuccessFetchInventoryCalled"));

    auto PlayerEquipmentDataString = Result.Data.FindRef(TEXT("PlayerEquipmentInventory")).Value;
    if (PlayerEquipmentDataString.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerEquipmentInventory is empty or not found."));
    }
    //auto DataTableRowNames = ConvertFromPlayFabFormat(PlayerEquipmentDataString);
    DataTableRowNames = ConvertFromPlayFabFormat(PlayerEquipmentDataString);

    if (Character)
    {
        UPlayerEquipment* PlayerEquipment = Cast<UPlayerEquipment>(Character->GetComponentByClass(UPlayerEquipment::StaticClass()));
        UDataTable* EquipmentDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Blueprints/DataTables/DT_PlayerEquipment.DT_PlayerEquipment")));
        if (!EquipmentDataTable)
        {
            UE_LOG(LogTemp, Error, TEXT("EquipmentDataTable is null in PlayFabManager."));
            return;
        }
        for (const FName& DataTableRowName : DataTableRowNames)
        {
            if (DataTableRowName.IsNone())
            {
                UE_LOG(LogTemp, Error, TEXT("Invalid DataTableRowName."));
                return;
            }
            if (EquipmentDataTable)
            {
                FEquipmentData* EquipmentData = EquipmentDataTable->FindRow<FEquipmentData>(DataTableRowName, TEXT("LookupEquipmentData"));
                //DataTableRowNames = ConvertFromPlayFabFormat(PlayerEquipmentDataString);

                if (EquipmentData)
                {
                    PlayerEquipment->AddEquipmentItem(*EquipmentData);
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("EquipmentDataTable is null in PlayFabManager"));
            }
            
        }

        //UE_LOG(LogTemp, Warning, TEXT("Broadcasting with %d items."), DataTableRowNames.Num());
        OnInventoryLoaded.Broadcast(DataTableRowNames);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Character null in playfabmanager"));
    }
}

void APlayFabManager::OnErrorFetchInventory(const PlayFab::FPlayFabCppError& Error)
{
    UE_LOG(LogTemp, Error, TEXT("Failed to update player equipment inventory on PlayFab. Error: %s"), *Error.GenerateErrorReport());
}


bool APlayFabManager::TryPlayFabUpdate(FName EssenceType, int32 NewCount)
{
    
    clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

    PlayFab::ClientModels::FUpdateUserDataRequest Request;
    Request.Data.Add(EssenceType.ToString(), FString::FromInt(NewCount));

    clientAPI->UpdateUserData(Request,
        PlayFab::UPlayFabClientAPI::FUpdateUserDataDelegate::CreateUObject(this, &APlayFabManager::OnSuccessUpdateEssence),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &APlayFabManager::OnErrorUpdateEssence));

    return true;
    
}

void APlayFabManager::OnSuccessUpdateEssence(const PlayFab::ClientModels::FUpdateUserDataResult& Result)
{
    //AIdleCharacter* Character = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    //OnPurchaseCompleted.Broadcast(true);
    //UE_LOG(LogTemp, Log, TEXT("Successfully updated essence count on PlayFab"));

    // Create an array of FEssenceCoffer to be broadcasted
    TArray<FEssenceCoffer> EssenceCofferArray;
    for (const auto& KeyValue : Character->CharacterInventory->EssenceAddedToCoffer)
    {
        FEssenceCoffer EssenceCoffer;
        EssenceCoffer.Key = KeyValue.Key;
        EssenceCoffer.Value = KeyValue.Value;
        EssenceCofferArray.Add(EssenceCoffer);
    }

    // Broadcast the OnEssenceTransferred event
    OnEssenceTransferred.Broadcast(EssenceCofferArray);
    // Increment the successful update count
    SuccessfulUpdateCount++;

    // Check if all essence types have been successfully updated
    if (SuccessfulUpdateCount == Character->CharacterInventory->EssenceAddedToCoffer.Num())
    {
        // Broadcast the OnEssenceTransferred event
        //OnEssenceTransferred.Broadcast(EssenceCofferArray);
        OnPurchaseCompleted.Broadcast(true);
    }
}

void APlayFabManager::OnErrorUpdateEssence(const PlayFab::FPlayFabCppError& Error)
{
    OnPurchaseCompleted.Broadcast(false);
    UE_LOG(LogTemp, Error, TEXT("Failed to update essence count on PlayFab. Error: %s"), *Error.GenerateErrorReport());
}

void APlayFabManager::OnSuccessUpdateInventory(const PlayFab::ClientModels::FUpdateUserDataResult& Result)
{
    //UE_LOG(LogTemp, Log, TEXT("Successfully updated player equipment inventory on PlayFab"));
    //LoadPlayerEquipmentInventory();
}

void APlayFabManager::OnErrorUpdateInventory(const PlayFab::FPlayFabCppError& Error)
{
    UE_LOG(LogTemp, Error, TEXT("Failed to update player equipment inventory on PlayFab. Error: %s"), *Error.GenerateErrorReport());
    // Optionally: trigger some in-game feedback to the player
}

TSharedPtr<FJsonObject> APlayFabManager::TMapToJsonObject(const TMap<FName, int32>& Map)
{
    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    for (const auto& KeyValue : Map)
    {
        JsonObject->SetNumberField(KeyValue.Key.ToString(), KeyValue.Value);
    }
    return JsonObject;
}

bool APlayFabManager::UpdateEssenceAddedToCofferOnPlayFab()
{
    clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

    //AIdleCharacter* Character = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (Character && Character->CharacterInventory)
    {
        // Serialize the updated EssenceAddedToCoffer map to a JSON object
        TSharedPtr<FJsonObject> JsonObject = TMapToJsonObject(Character->CharacterInventory->EssenceAddedToCoffer);
        FString UpdatedDataString;
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&UpdatedDataString);
        FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

        //UE_LOG(LogTemp, Log, TEXT("Updating PlayFab with essence data: %s"), *UpdatedDataString);

        // Create a request to update the PlayFab user data
        PlayFab::ClientModels::FUpdateUserDataRequest UpdateRequest;
        UpdateRequest.Data.Add(TEXT("EssenceAddedToCoffer"), UpdatedDataString);
        clientAPI->UpdateUserData(UpdateRequest,
            PlayFab::UPlayFabClientAPI::FUpdateUserDataDelegate::CreateUObject(this, &APlayFabManager::OnSuccessUpdateEssence),
            PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &APlayFabManager::OnErrorUpdateEssence));

        return true;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Character or inventory are null ptr in PlayFabManager"));
        return false;
    }
}

void APlayFabManager::InitializeEssenceCounts()
{
    //Initializes all essence counts to zero before the update so none are missed (and for new players)
    TArray<FName> AllEssenceTypes = {
        FName(TEXT("Wisdom")),
        FName(TEXT("Temperance")),
        FName(TEXT("Justice")),
        FName(TEXT("Courage")),
        FName(TEXT("Legendary"))
    };

    //AIdleCharacter* Character = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (Character && Character->CharacterInventory)
    {
        for (const FName& EssenceType : AllEssenceTypes)
        {
            Character->CharacterInventory->EssenceAddedToCoffer.FindOrAdd(EssenceType) = 0;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Character or inventory are null ptr in InitializeEssenceCounts"));
    }
}

FString APlayFabManager::ConvertToPlayFabFormat(const TArray<FEquipmentData>& EquipmentDataArray)
{
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    Writer->WriteArrayStart();
    for (const FEquipmentData& EquipmentData : EquipmentDataArray)
    {
        Writer->WriteObjectStart();
        Writer->WriteValue(TEXT("DataTableRowName"), EquipmentData.Name);
        Writer->WriteObjectEnd();

        // Log the name being written
        //UE_LOG(LogTemp, Warning, TEXT("ConvertToPlayFabFormat: Writing EquipmentData.Name: %s"), *EquipmentData.Name);
    }
    Writer->WriteArrayEnd();

    Writer->Close();

    // Log the final output string
    //UE_LOG(LogTemp, Warning, TEXT("ConvertToPlayFabFormat: Final OutputString: %s"), *OutputString);

    return OutputString;
}

TArray<FName> APlayFabManager::ConvertFromPlayFabFormat(const FString& PlayFabData)
{
    TArray<FName> OutputArray;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(PlayFabData);

    //UE_LOG(LogTemp, Warning, TEXT("ConvertFromPlayFabFormat: Input PlayFabData: %s"), *PlayFabData);

    TArray<TSharedPtr<FJsonValue>> JsonArray;
    if (FJsonSerializer::Deserialize(Reader, JsonArray))
    {
        //UE_LOG(LogTemp, Warning, TEXT("ConvertFromPlayFabFormat: Successfully deserialized PlayFabData."));
        for (TSharedPtr<FJsonValue> Value : JsonArray)
        {
            TSharedPtr<FJsonObject> Object = Value->AsObject();
            if (Object.IsValid())
            {
                FName DataTableRowName = FName(*Object->GetStringField(TEXT("DataTableRowName")));
                OutputArray.Add(DataTableRowName);
                //UE_LOG(LogTemp, Warning, TEXT("ConvertFromPlayFabFormat: Added DataTableRowName: %s"), *DataTableRowName.ToString());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("ConvertFromPlayFabFormat: Object is not valid."));
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ConvertFromPlayFabFormat: Failed to deserialize PlayFabData."));
    }

    //UE_LOG(LogTemp, Warning, TEXT("ConvertFromPlayFabFormat: Number of items in OutputArray: %d"), OutputArray.Num());
    return OutputArray;
}
