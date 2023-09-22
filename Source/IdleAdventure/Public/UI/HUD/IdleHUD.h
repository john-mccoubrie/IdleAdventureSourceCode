

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "IdleHUD.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
class UOverlayWidgetController;
class UIdleUserWidget;
struct FWidgetControllerParams;

UCLASS()
class IDLEADVENTURE_API AIdleHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
		TObjectPtr<UIdleUserWidget>  OverlayWidget;

	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

private:

	UPROPERTY(EditAnywhere)
		TSubclassOf<UIdleUserWidget> OverlayWidgetClass;


	UPROPERTY()
		TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
};
