


#include "Inventory/Inventory.h"
#include "Character/IdleCharacter.h"
#include <Kismet/GameplayStatics.h>


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
        boolOnItemAdded.Broadcast(true);
        OnItemAdded.Broadcast(Item->EssenceRarity);
        AIdleCharacter* Character = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        Character->EssenceCount++;
        EssenceCount.FindOrAdd(Item->EssenceRarity) += 1;
        for (const auto& EssenceEntry : EssenceCount)
        {
            UE_LOG(LogTemp, Warning, TEXT("Essence: %s, Count: %d"), *EssenceEntry.Key.ToString(), EssenceEntry.Value);
        }
        //UE_LOG(LogTemp, Warning, TEXT("Item Added in Inventory class"));
    }
}

void AInventory::RemoveItem(UItem* Item)
{
    Items.Remove(Item);
    boolOnItemAdded.Broadcast(false);
    OnItemRemoved.Broadcast(Item->EssenceRarity);


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

void AInventory::TransferAndClearEssenceCounts()
{
    // Transfer the essence counts
    for (const auto& EssenceEntry : EssenceCount)
    {
        EssenceAddedToCoffer.FindOrAdd(EssenceEntry.Key) += EssenceEntry.Value;
        UE_LOG(LogTemp, Warning, TEXT("Essence: %s, Count: %d"), *EssenceEntry.Key.ToString(), EssenceEntry.Value);
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

    // Clear the EssenceCount map
    EssenceCount.Empty();

    for (const auto& EssenceEntry : EssenceAddedToCoffer)
    {
        UE_LOG(LogTemp, Warning, TEXT("Total Essence: %s, Count: %d"), *EssenceEntry.Key.ToString(), EssenceEntry.Value);
    }
}

int32 AInventory::GetEssenceCount(FName EssenceType) const
{
    return EssenceCount.Contains(EssenceType) ? EssenceCount[EssenceType] : 0;
}

TMap<FName, int32> AInventory::GetAllEssenceCounts() const
{
    return EssenceCount;
}



