


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/IdleAttributeSet.h"

UOverlayWidgetController* UOverlayWidgetController::GetOverlayWidgetController() const
{
	return nullptr;
}

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(AttributeSet);

	OnEXPChanged.Broadcast(IdleAttributeSet->GetWoodcutExp());
	OnMaxEXPChanged.Broadcast(IdleAttributeSet->GetMaxWoodcutExp());

}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		IdleAttributeSet->GetWoodcutExpAttribute()).AddUObject(this, &UOverlayWidgetController::EXPChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		IdleAttributeSet->GetMaxWoodcutExpAttribute()).AddUObject(this, &UOverlayWidgetController::MaxEXPChanged);
}

void UOverlayWidgetController::EXPChanged(const FOnAttributeChangeData& Data) const
{
	OnEXPChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxEXPChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxEXPChanged.Broadcast(Data.NewValue);
}
