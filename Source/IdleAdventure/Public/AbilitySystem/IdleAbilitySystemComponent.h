

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "IdleAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API UIdleAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
};
