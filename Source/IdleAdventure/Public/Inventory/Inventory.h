

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "GameFramework/Actor.h"
#include "Inventory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdated, bool, bAdded);


UCLASS()
class IDLEADVENTURE_API AInventory : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInventory();

    // Add an item to the inventory
    UFUNCTION(BlueprintCallable, Category = "Inventory")
        void AddItem(UItem* Item);

    // Remove an item from the inventory
    UFUNCTION(BlueprintCallable, Category = "Inventory")
        void RemoveItem(UItem* Item);

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
        FOnInventoryUpdated OnInventoryUpdated;


    // List of items in the inventory
    UPROPERTY(EditAnywhere, Category = "Inventory")
        TArray<UItem*> Items;

};
