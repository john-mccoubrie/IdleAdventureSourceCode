


#include "PlayerEquipment/EquipmentManagerLibrary.h"

UEquipmentManager* UEquipmentManagerLibrary::GetEquipmentManager()
{
	UEquipmentManager* EquipmentManager = &UEquipmentManager::Get();
    if (EquipmentManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetEquipmentManager: EquipmentManager is valid"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GetEquipmentManager: EquipmentManager is NULL"));
    }
    return EquipmentManager;
}
