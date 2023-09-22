

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IdleUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API UIdleUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		void SetWidgetController(UObject* InWidgetController);

	UPROPERTY(BlueprintReadOnly)
		TObjectPtr<UObject> WidgetController;

protected:
	UFUNCTION(BlueprintImplementableEvent)
		void WidgetControllerSet();
	
};
