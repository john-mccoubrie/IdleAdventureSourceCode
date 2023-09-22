


#include "Inventory/Inventory.h"


// Sets default values
AInventory::AInventory()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    //UE_LOG(LogTemp, Warning, TEXT("Inventory class constructor called"));
}

void AInventory::AddItem(UItem* Item)
{
    if (Item != nullptr)
    {
        Items.Add(Item);
        OnInventoryUpdated.Broadcast(true);
        //UE_LOG(LogTemp, Warning, TEXT("Item Added in Inventory class"));
    }
}

void AInventory::RemoveItem(UItem* Item)
{
    Items.Remove(Item);
    OnInventoryUpdated.Broadcast(false);
}



