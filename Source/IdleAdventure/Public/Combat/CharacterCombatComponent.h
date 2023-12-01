

#pragma once

#include "CoreMinimal.h"
#include "Combat/BaseCombatComponent.h"
#include "CharacterCombatComponent.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API UCharacterCombatComponent : public UBaseCombatComponent
{
	GENERATED_BODY()
	
	virtual void HandleDeath() override;
	virtual void TakeDamage(float amount) override;
};
