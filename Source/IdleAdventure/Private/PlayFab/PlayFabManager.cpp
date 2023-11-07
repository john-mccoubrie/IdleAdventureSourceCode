


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
    FetchCompletedQuestsData();
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
    if (!World || !World->IsValidLowLevelFast() || World->bIsTearingDown)
    {
        // The world is not valid or is being destroyed, return nullptr.
        return nullptr;
    }

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

void APlayFabManager::SaveQuestStatsToPlayFab(const FString& QuestID, const FString& CompletionDate)
{
    // Store the quest ID and completion date in member variables
    PendingQuestID = QuestID;
    PendingCompletionDate = FDateTime::UtcNow().ToString();;

    // Fetch the existing completed quests data first
    PlayFab::ClientModels::FGetUserDataRequest GetRequest;
    GetRequest.Keys.Add(TEXT("CompletedQuests"));
    clientAPI->GetUserData(GetRequest,
        PlayFab::UPlayFabClientAPI::FGetUserDataDelegate::CreateUObject(this, &APlayFabManager::OnFetchedCompletedQuestsBeforeSaving),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &APlayFabManager::OnFetchCompletedQuestsDataFailure)
    );
}

void APlayFabManager::OnFetchedCompletedQuestsBeforeSaving(const PlayFab::ClientModels::FGetUserDataResult& Result)
{
    FString QuestID = PendingQuestID;
    FString CompletionDate = PendingCompletionDate;

    // Create a JSON object that represents a single quest completion
    TSharedPtr<FJsonObject> QuestCompletionJsonObject = MakeShared<FJsonObject>();
    QuestCompletionJsonObject->SetStringField(TEXT("LastCompleted"), CompletionDate);

    TSharedPtr<FJsonObject> CompletedQuestsJsonObject;
    if (Result.Data.Contains(TEXT("CompletedQuests")))
    {
        // Existing data found, parse it
        const FString& CompletedQuestsData = Result.Data[TEXT("CompletedQuests")].Value;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(CompletedQuestsData);
        if (FJsonSerializer::Deserialize(Reader, CompletedQuestsJsonObject) && CompletedQuestsJsonObject.IsValid())
        {
            // Now, update the specific quest's last completion date
            CompletedQuestsJsonObject->SetObjectField(QuestID, QuestCompletionJsonObject);
        }
    }
    else
    {
        // No existing data, create a new JSON object
        CompletedQuestsJsonObject = MakeShared<FJsonObject>();
        CompletedQuestsJsonObject->SetObjectField(QuestID, QuestCompletionJsonObject);
    }

    // Serialize the updated object back into a string
    FString UpdatedCompletedQuestsData;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&UpdatedCompletedQuestsData);
    FJsonSerializer::Serialize(CompletedQuestsJsonObject.ToSharedRef(), Writer);

    // Now, update PlayFab with the new data
    PlayFab::ClientModels::FUpdateUserDataRequest UpdateRequest;
    UpdateRequest.Data.Add(TEXT("CompletedQuests"), UpdatedCompletedQuestsData);

    clientAPI->UpdateUserData(UpdateRequest,
        PlayFab::UPlayFabClientAPI::FUpdateUserDataDelegate::CreateUObject(this, &APlayFabManager::OnUpdateQuestStatsSuccess),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &APlayFabManager::OnUpdateQuestStatsFailure)
    );
}

void APlayFabManager::OnUpdateQuestStatsSuccess(const PlayFab::ClientModels::FUpdateUserDataResult& Result)
{
    UE_LOG(LogTemp, Log, TEXT("User data updated successfully in update quest stats."));
}

void APlayFabManager::OnUpdateQuestStatsFailure(const PlayFab::FPlayFabCppError& ErrorResult)
{
    UE_LOG(LogTemp, Error, TEXT("Failed to save quest stats to playfab: %s"), *ErrorResult.ErrorMessage);
}

void APlayFabManager::CanAcceptQuest(UQuest* Quest, AIdleCharacter* Player)
{
    if (!Quest || !Player)
    {
        UE_LOG(LogTemp, Warning, TEXT("CanAcceptQuest called with invalid parameters."));
        return;
    }

    // Check if the quest exists in the completed quests data and if it needs to be reset
    FString* LastCompletedDatePtr = PlayerCompletedQuestsData.Find(Quest->QuestID);
    if (LastCompletedDatePtr)
    {
        // The quest has been completed before, check if it needs reset (e.g., if it's a daily quest)
        if (!NeedsReset(*LastCompletedDatePtr))
        {
            // The player has already completed this quest, and it doesn't need a reset
            Player->NotifyQuestCompletionStatus(Quest->QuestID, false);
            return;
        }
    }

    // If the quest hasn't been completed, or it has been completed but needs a reset, it can be accepted
    Player->NotifyQuestCompletionStatus(Quest->QuestID, true);
}

void APlayFabManager::CompleteQuest(UQuest* Quest)
{
    // Mark the quest as completed in your game logic
    UE_LOG(LogTemp, Warning, TEXT("Complete quest called in playfabmanager."));

    // Add the quest and the completion timestamp to the CompletedQuests map
    CompletedQuests.Add(Quest->QuestID, FDateTime::UtcNow().ToString());

    // Save all completed quests to PlayFab
    SaveQuestStatsToPlayFab(Quest->QuestID, Quest->Version); // Updated to call without parameters
}

void APlayFabManager::GetCompletedQuestVersion(FString QuestID, AIdleCharacter* Player)
{
    PlayFab::ClientModels::FGetUserDataRequest Request;
    Request.Keys = { QuestID }; // Requesting the data associated with the specific quest

    // Pass the Player as part of the context to the callback
    clientAPI->GetUserData(Request,
        PlayFab::UPlayFabClientAPI::FGetUserDataDelegate::CreateUObject(this, &APlayFabManager::OnGetQuestVersionSuccess, Player),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &APlayFabManager::OnGetQuestVersionFailure)
    );
}

void APlayFabManager::OnGetQuestVersionSuccess(const PlayFab::ClientModels::FGetUserDataResult& Result, AIdleCharacter* Player)
{
    UE_LOG(LogTemp, Warning, TEXT("Successfully retrieved user data."));

    // Process the result and determine if the quest can be accepted
    for (const auto& Entry : Result.Data)
    {
        FString QuestID = Entry.Key;
        FString QuestVersion = Entry.Value.Value;
        FDateTime LastUpdatedTime = Entry.Value.LastUpdated; // This is your FDateTime
        FString LastCompletedDate = LastUpdatedTime.ToString();

        // Check if the quest is completed and if it needs to be reset (if it's a daily quest)
        if (Player->HasQuestWithVersion(QuestID, QuestVersion) && !NeedsReset(LastCompletedDate))
        {
            // The player has already completed this version of the quest and it doesn't need a reset
            Player->NotifyQuestCompletionStatus(QuestID, false);
            UE_LOG(LogTemp, Warning, TEXT("notify quest completion status passing false."));
        }
        else
        {
            // The player can accept this quest or the quest has been reset
            Player->NotifyQuestCompletionStatus(QuestID, true);
            UE_LOG(LogTemp, Warning, TEXT("notify quest completion status passing true."));
        }
    }
}

void APlayFabManager::OnGetQuestVersionFailure(const PlayFab::FPlayFabCppError& ErrorResult)
{
    UE_LOG(LogTemp, Warning, TEXT("Mark quest as completed."));
    UE_LOG(LogTemp, Error, TEXT("Failed to retrieve user data: %s"), *ErrorResult.ErrorMessage);
    // Handle failure, e.g., retry or inform the user
}

void APlayFabManager::MarkQuestAsCompleted(FString QuestID, FString Version)
{
    /*
    UE_LOG(LogTemp, Warning, TEXT("Mark Quest as completed called in playfabmanager."));
    // Create a map containing the quest completion data
    TMap<FString, FString> CompletedQuests;
    CompletedQuests.Add(QuestID, Version);

    // Save this data to PlayFab
    SaveQuestStatsToPlayFab(QuestID);
    */
}

void APlayFabManager::CheckIfQuestCompleted(UQuest* Quest, AIdleCharacter* Player)
{
    // Fetch the completed quest version for the given quest
    GetCompletedQuestVersion(Quest->QuestID, Player);
}

bool APlayFabManager::NeedsReset(const FString& LastCompletedDate)
{
    FDateTime LastDate;
    FDateTime::Parse(LastCompletedDate, LastDate);
    FDateTime CurrentDate = FDateTime::UtcNow(); // Assuming you're using UTC time everywhere

    // This will reset the quest every new day (UTC time)
    return LastDate.GetDate() != CurrentDate.GetDate();
}

void APlayFabManager::FetchCompletedQuestsData()
{
    // Request for completed quests data
    PlayFab::ClientModels::FGetUserDataRequest Request;
    Request.Keys.Add(TEXT("CompletedQuests"));

    clientAPI->GetUserData(Request,
        PlayFab::UPlayFabClientAPI::FGetUserDataDelegate::CreateUObject(this, &APlayFabManager::OnFetchCompletedQuestsDataSuccess),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &APlayFabManager::OnFetchCompletedQuestsDataFailure)
    );
}

void APlayFabManager::OnFetchCompletedQuestsDataSuccess(const PlayFab::ClientModels::FGetUserDataResult& Result)
{
    UE_LOG(LogTemp, Error, TEXT("Got new quest data"));

    const FString CompletedQuestsKey = TEXT("CompletedQuests");
    const FString LastCompletedField = TEXT("LastCompleted");

    // Check if the CompletedQuests key is in the result data
    if (Result.Data.Contains(CompletedQuestsKey))
    {
        const auto& CompletedQuestsJsonValue = Result.Data[CompletedQuestsKey];

        // Parse the JSON string for the CompletedQuests object
        TSharedPtr<FJsonObject> CompletedQuestsJsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(CompletedQuestsJsonValue.Value);
        if (FJsonSerializer::Deserialize(Reader, CompletedQuestsJsonObject) && CompletedQuestsJsonObject.IsValid())
        {
            // Now iterate over the CompletedQuests object to get each quest's completion time
            for (const auto& QuestEntry : CompletedQuestsJsonObject->Values)
            {
                const FString QuestID = QuestEntry.Key;
                const TSharedPtr<FJsonValue> QuestValue = QuestEntry.Value;

                // Make sure this value is an object
                if (QuestValue.IsValid() && QuestValue->Type == EJson::Object)
                {
                    TSharedPtr<FJsonObject> QuestObject = QuestValue->AsObject();
                    FString LastCompletedDate = QuestObject->GetStringField(LastCompletedField);

                    PlayerCompletedQuestsData.Add(QuestID, LastCompletedDate);
                    UE_LOG(LogTemp, Log, TEXT("Quest %s completed on: %s"), *QuestID, *LastCompletedDate);
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to deserialize CompletedQuests data."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("CompletedQuests key not found in Result.Data"));
    }

    // Optionally, notify the game that quest data is ready to be used
    OnQuestDataReady.Broadcast();
}

void APlayFabManager::OnFetchCompletedQuestsDataFailure(const PlayFab::FPlayFabCppError& ErrorResult)
{
    UE_LOG(LogTemp, Error, TEXT("Failed to retrieve user data: %s"), *ErrorResult.ErrorMessage);
}