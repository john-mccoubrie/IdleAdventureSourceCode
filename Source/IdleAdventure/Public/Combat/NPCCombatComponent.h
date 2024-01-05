

#pragma once

#include "CoreMinimal.h"
#include "Combat/BaseCombatComponent.h"
#include "NPCCombatComponent.generated.h"


class AHealthPotion;

UCLASS()
class IDLEADVENTURE_API UNPCCombatComponent : public UBaseCombatComponent
{
	GENERATED_BODY()
	
public:

	virtual void HandleDeath() override;
	virtual void TakeDamage(float amount, float level) override;
	void StopDamageCheckTimer();
	void DestroyOwner();

	UPROPERTY(EditDefaultsOnly, Category = "HealthPotion")
	TSubclassOf<AHealthPotion> HealthPotionBlueprint;

private:
	FTimerHandle DamageCheckTimer;
	float PendingDamage;
	void DamageCheck();

};
