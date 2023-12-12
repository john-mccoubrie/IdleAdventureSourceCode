

#pragma once

#include "CoreMinimal.h"
#include "Combat/BaseCombatComponent.h"
#include "DemonBossCombatComponent.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API UDemonBossCombatComponent : public UBaseCombatComponent
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
	virtual void HandleDeath() override;
	virtual void TakeDamage(float amount) override;
	void StopDamageCheckTimer();

private:
	FTimerHandle DamageCheckTimer;
	void DamageCheck();
	float PendingDamage;
};
