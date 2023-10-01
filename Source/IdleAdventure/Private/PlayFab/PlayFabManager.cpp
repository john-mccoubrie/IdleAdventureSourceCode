


#include "PlayFab/PlayFabManager.h"
#include <Character/IdleCharacter.h>
#include <Kismet/GameplayStatics.h>
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"

//Define the static member variable
APlayFabManager* APlayFabManager::SingletonInstance = nullptr;

APlayFabManager::APlayFabManager()
{
    
}

void APlayFabManager::BeginPlay()
{
    Super::BeginPlay();

    // Ensure the singleton instance is set on BeginPlay
    if (!SingletonInstance)
    {
        SingletonInstance = this;
    }
}

APlayFabManager* APlayFabManager::GetInstance()
{
    return SingletonInstance;  
}


bool APlayFabManager::PurchaseEquipment(const FString& EquipmentName, const FEquipmentData& EquipmentData)
{
    // TODO: Implement logic to update player essence count on PlayFab
    return false;
}

bool APlayFabManager::UpdatePlayFabEssenceCount(const FEquipmentData& EquipmentData)
{
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

    // Ensure all essence types have enough essence to cover the costs
    for (int32 i = 0; i < EssenceTypes.Num(); ++i)
    {
        AIdleCharacter* Character = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        if (Character && Character->CharacterInventory)
        {
            int32* CurrentEssenceCount = Character->CharacterInventory->EssenceAddedToCoffer.Find(EssenceTypes[i]);
            if (CurrentEssenceCount && *CurrentEssenceCount < Costs[i])
            {
                UE_LOG(LogTemp, Warning, TEXT("Insufficient essence of type: %s"), *EssenceTypes[i].ToString());
                return false;
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Character or inventory are null ptr in PlayFabManager"));
        }
    }

    // Deduct the costs and update on PlayFab
    for (int32 i = 0; i < EssenceTypes.Num(); ++i)
    {
        AIdleCharacter* Character = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        if (Character && Character->CharacterInventory)
        {
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
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Character or inventory are null ptr in PlayFabManager"));
        }
        
    }

    // Update EssenceAddedToCoffer on PlayFab after updating essence counts
    UpdateEssenceAddedToCofferOnPlayFab();

    return true;
    
}

bool APlayFabManager::UpdatePlayerEquipmentInventory(const TArray<FEquipmentData>& NewPlayerEquipmentInventory)
{
    // TODO: Implement logic to update player inventory on PlayFab
    return false;
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
    UE_LOG(LogTemp, Log, TEXT("Successfully updated essence count on PlayFab"));
}

void APlayFabManager::OnErrorUpdateEssence(const PlayFab::FPlayFabCppError& Error)
{
    UE_LOG(LogTemp, Error, TEXT("Failed to update essence count on PlayFab. Error: %s"), *Error.GenerateErrorReport());
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

    AIdleCharacter* Character = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (Character && Character->CharacterInventory)
    {
        // Serialize the updated EssenceAddedToCoffer map to a JSON object
        TSharedPtr<FJsonObject> JsonObject = TMapToJsonObject(Character->CharacterInventory->EssenceAddedToCoffer);
        FString UpdatedDataString;
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&UpdatedDataString);
        FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

        UE_LOG(LogTemp, Log, TEXT("Updating PlayFab with essence data: %s"), *UpdatedDataString);

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

    AIdleCharacter* Character = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
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
