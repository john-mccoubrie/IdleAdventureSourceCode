


#include "PlayerEquipment/EquipmentManager.h"
#include <PlayerEquipment/PlayerEquipment.h>
#include <AbilitySystem/IdleAttributeSet.h>

TArray<FEquipmentData> UEquipmentManager::GetUnlockedEquipment() const
{
    return UnlockedEquipment;
}

UEquipmentManager::UEquipmentManager()
{
    //previously was "DT_Equipment" I might have messed up the AllEquipmentDataTable vs Unlocked data, etc.
    static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("/Game/Blueprints/Character/Equipment/DT_PlayerEquipment.DT_PlayerEquipment"));
    if (DataTable.Succeeded())
    {
        AllEquipmentDataTable = DataTable.Object;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load data table"));
    }
}

UEquipmentManager* GEquipmentManager = nullptr;

UEquipmentManager& UEquipmentManager::Get()
{
    if (GEquipmentManager == nullptr)
    {
        GEquipmentManager = NewObject<UEquipmentManager>();
        GEquipmentManager->AddToRoot();
        //UE_LOG(LogTemp, Warning, TEXT("Singleton created"));
    }

    return *GEquipmentManager;
}

void UEquipmentManager::UnlockEquipment(float PlayerLevel)
{
    static const FString ContextString(TEXT("General"));
    for (auto& Row : AllEquipmentDataTable->GetRowMap())
    {
        FEquipmentData* EquipmentData = (FEquipmentData*)Row.Value;
        //UE_LOG(LogTemp, Warning, TEXT("Equipment data level required: %f"), EquipmentData->LevelRequired);
        //UE_LOG(LogTemp, Warning, TEXT("Player Level: %f"), PlayerLevel);
        if (PlayerLevel >= EquipmentData->LevelRequired)
        {
            // Check if UnlockedEquipment already contains this piece of equipment
            bool bAlreadyUnlocked = false;
            for (const FEquipmentData& UnlockedData : UnlockedEquipment)
            {
                if (UnlockedData.Name == EquipmentData->Name)  // Assuming that each equipment has a unique Id field
                {
                    bAlreadyUnlocked = true;
                    //UE_LOG(LogTemp, Warning, TEXT("This equipment is already unlocked: %s"), *EquipmentData->Name);
                    break;
                }
            }

            // If not already unlocked, add it to UnlockedEquipment
            if (!bAlreadyUnlocked)
            {
                UnlockedEquipment.Add(*EquipmentData);
                //UE_LOG(LogTemp, Warning, TEXT("Added equipment: %s"), *EquipmentData->Name);
            }
        }
        else
        {
            //UE_LOG(LogTemp, Warning, TEXT("Player Level too low!"));
        }
    }
}
