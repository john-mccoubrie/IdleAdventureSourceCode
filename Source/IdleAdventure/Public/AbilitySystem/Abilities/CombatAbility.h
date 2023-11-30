

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CombatAbility.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API UCombatAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
