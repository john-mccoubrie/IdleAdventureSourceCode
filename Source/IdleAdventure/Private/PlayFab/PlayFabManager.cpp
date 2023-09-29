


#include "PlayFab/PlayFabManager.h"
#include <Character/IdleCharacter.h>
#include <Kismet/GameplayStatics.h>
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"

//Define the static member variable
UPlayFabManager* UPlayFabManager::GPlayFabManager = nullptr;

UPlayFabManager::UPlayFabManager()
{
    
}

UPlayFabManager* UPlayFabManager::GetInstance()
{
    
    if (!GPlayFabManager)
    {
        GPlayFabManager = NewObject<UPlayFabManager>();
        GPlayFabManager->AddToRoot();  // Prevent garbage collection
    }
    return GPlayFabManager;
    
}


bool UPlayFabManager::PurchaseEquipment(const FString& EquipmentName, const FEquipmentData& EquipmentData)
{
    // TODO: Implement logic to update player essence count on PlayFab
    return false;
}

bool UPlayFabManager::UpdatePlayFabEssenceCount(const FEquipmentData& EquipmentData)
{
    
    AIdleCharacter* Character = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
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
        
        
        int32* CurrentEssenceCount = Character->CharacterInventory->EssenceAddedToCoffer.Find(EssenceTypes[i]);
        if (!CurrentEssenceCount || *CurrentEssenceCount < Costs[i])
        {
            UE_LOG(LogTemp, Warning, TEXT("Insufficient essence of type: %s"), *EssenceTypes[i].ToString());
            return false;
        }
    }

    // Deduct the costs and update on PlayFab
    for (int32 i = 0; i < EssenceTypes.Num(); ++i)
    {
        int32* CurrentEssenceCount = Character->CharacterInventory->EssenceAddedToCoffer.Find(EssenceTypes[i]);
        *CurrentEssenceCount -= Costs[i];

        // Now update the essence count on PlayFab
        // Assume UpdatePlayFabEssenceCount is a method that handles updating essence count on PlayFab
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

    return true;
    
}

bool UPlayFabManager::UpdatePlayerEquipmentInventory(const TArray<FEquipmentData>& NewPlayerEquipmentInventory)
{
    // TODO: Implement logic to update player inventory on PlayFab
    return false;
}


bool UPlayFabManager::TryPlayFabUpdate(FName EssenceType, int32 NewCount)
{
    
    clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

    PlayFab::ClientModels::FUpdateUserDataRequest Request;
    Request.Data.Add(EssenceType.ToString(), FString::FromInt(NewCount));

    clientAPI->UpdateUserData(Request,
        PlayFab::UPlayFabClientAPI::FUpdateUserDataDelegate::CreateUObject(this, &UPlayFabManager::OnSuccessUpdateEssence),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UPlayFabManager::OnErrorUpdateEssence));

    return true;
    
}

void UPlayFabManager::OnSuccessUpdateEssence(const PlayFab::ClientModels::FUpdateUserDataResult& Result)
{
    UE_LOG(LogTemp, Log, TEXT("Successfully updated essence count on PlayFab"));
}

void UPlayFabManager::OnErrorUpdateEssence(const PlayFab::FPlayFabCppError& Error)
{
    UE_LOG(LogTemp, Error, TEXT("Failed to update essence count on PlayFab. Error: %s"), *Error.GenerateErrorReport());
}
