

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "Item.generated.h"

USTRUCT(BlueprintType)
struct FEssenceData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;
};
//UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
UCLASS()
class IDLEADVENTURE_API UItem : public UObject
{
	GENERATED_BODY()
public:

    UItem();

    //data table row is turned into FName to then pass to blueprints(UI)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Data")
    FName EssenceRarity;

    //not sure if any of the below are used

    //UItem();
    // Name of the Item
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FString Name;

    // Icon for the UI
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    UTexture2D* Icon;

    // Text for using the item (Equip, use, etc.)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FText UseActionText;

    // Description for the item
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (MultiLine = true))
        FText ItemDescription;

    // The inventory that owns this item
    UPROPERTY()
        class AInventory* OwningInventory;

       

    //virtual void Use(class AInventorySystemCharacter* Character) PURE_VIRTUAL(UItem, );

    // Some additional properties can be added here like description, value, etc.

};
