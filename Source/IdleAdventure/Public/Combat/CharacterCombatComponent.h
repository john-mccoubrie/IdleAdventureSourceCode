

#pragma once

#include "CoreMinimal.h"
#include "Combat/BaseCombatComponent.h"
#include "CharacterCombatComponent.generated.h"


UCLASS()
class IDLEADVENTURE_API UCharacterCombatComponent : public UBaseCombatComponent
{
	GENERATED_BODY()
public:

	virtual void HandleDeath() override;
	virtual void TakeDamage(float amount, float level) override;
	virtual void AddHealth(float HealthToAdd) override;	
};
