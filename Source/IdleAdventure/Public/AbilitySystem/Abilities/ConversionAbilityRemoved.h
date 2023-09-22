

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEffectRemoved.h"
#include "ConversionAbilityRemoved.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API UConversionAbilityRemoved : public UAbilityTask_WaitGameplayEffectRemoved
{
	GENERATED_BODY()
	
public:

	static UConversionAbilityRemoved* WaitForGameplayEffectRemoved(UGameplayAbility* OwningAbility, FActiveGameplayEffectHandle InHandle);

	virtual void NotifyOnGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo);
};
