

#pragma once

#include "CoreMinimal.h"
//#include <Player/IdlePlayerState.h>
#include "Abilities/GameplayAbility.h"
#include "IdleGameplayAbility.generated.h"

//class AbilitySystemComponent;
//class GameplayEffect;
/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API UIdleGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UIdleGameplayAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
