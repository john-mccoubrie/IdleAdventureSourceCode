

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

public:

	virtual void BeginPlay() override;
	virtual void HandleDeath() override;
	virtual void TakeDamage(float amount, float level) override;
	virtual void DamageCheck() override;
	void StopDamageCheckTimer();

	// Method to initiate the tailspin attack
	void InitiateTailspinAttack();

	// Method to check if player is in range for the tailspin attack
	void CheckForPlayerInRange();

	// Method to move the boss towards the player
	void MoveTowardsPlayer();

	// Method to apply damage to the player during the tailspin attack
	void ApplyTailspinDamage();

	// Method to stop applying tailspin damage
	void StopTailspinDamage();

	void StopTailspinAttack();

	void InitializeTailspinAttackTimer();

	bool bIsTailspinActive;

	void DestroyOwner();

private:
	//FTimerHandle DamageCheckTimer;
	//void DamageCheck();
	//float PendingDamage;

	// Timers for the tailspin attack
	FTimerHandle TailspinAttackTimer;
	FTimerHandle TailspinDamageTimer;

	// Tailspin attack parameters
	float TailspinAttackIntervalMin;
	float TailspinAttackIntervalMax;
	float TailspinDamageInterval;
	float TailspinRange;
};
