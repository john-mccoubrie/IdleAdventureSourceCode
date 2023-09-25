

#pragma once

#include "EquipmentManager.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EquipmentManagerLibrary.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API UEquipmentManagerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	static UEquipmentManager* GetEquipmentManager();
	
};
