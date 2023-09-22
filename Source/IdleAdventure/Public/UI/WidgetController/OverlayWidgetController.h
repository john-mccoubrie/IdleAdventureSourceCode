

#pragma once

#include "CoreMinimal.h"
#include "UI/IdleWidgetController.h"
#include "OverlayWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEXPChangedSignature, float, NewEXP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxEXPChangedSignature, float, NewMaxEXP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUp, float, newLevel);

UCLASS(BlueprintType, Blueprintable)
class IDLEADVENTURE_API UOverlayWidgetController : public UIdleWidgetController
{
	GENERATED_BODY()
	
public:

	class UOverlayWidgetController* GetOverlayWidgetController() const;

	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
		FOnEXPChangedSignature OnEXPChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
		FOnMaxEXPChangedSignature OnMaxEXPChanged;

	

protected:
	void EXPChanged(const FOnAttributeChangeData& Data) const;
	void MaxEXPChanged(const FOnAttributeChangeData& Data) const;

};
