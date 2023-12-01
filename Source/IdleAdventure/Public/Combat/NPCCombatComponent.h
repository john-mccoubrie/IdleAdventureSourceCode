

#pragma once

#include "CoreMinimal.h"
#include "Combat/BaseCombatComponent.h"
#include "NPCCombatComponent.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API UNPCCombatComponent : public UBaseCombatComponent
{
	GENERATED_BODY()
	
public:

	virtual void HandleDeath() override;
	virtual void TakeDamage(float amount) override;
	void StopDamageCheckTimer();

private:
	FTimerHandle DamageCheckTimer;
	float PendingDamage;
	void DamageCheck();

};
