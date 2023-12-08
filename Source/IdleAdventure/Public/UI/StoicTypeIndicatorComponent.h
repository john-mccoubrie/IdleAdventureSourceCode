

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "StoicTypeIndicatorComponent.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API UStoicTypeIndicatorComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Stoic Type")
	void SetStoicType(const FString& StoicType);
};
