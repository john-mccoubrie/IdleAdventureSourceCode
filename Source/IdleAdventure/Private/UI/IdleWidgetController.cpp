


#include "UI/IdleWidgetController.h"

void UIdleWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UIdleWidgetController::BroadcastInitialValues()
{

}

void UIdleWidgetController::BindCallbacksToDependencies()
{
	
}
