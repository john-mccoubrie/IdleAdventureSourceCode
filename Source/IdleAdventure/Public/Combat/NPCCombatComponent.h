

#pragma once

#include "CoreMinimal.h"
#include "Combat/BaseCombatComponent.h"
#include "NPCCombatComponent.generated.h"


class AHealthPotion;
class AEnemyBase;

UCLASS()
class IDLEADVENTURE_API UNPCCombatComponent : public UBaseCombatComponent
{
	GENERATED_BODY()
	
public:

	virtual void HandleDeath() override;
	virtual void TakeDamage(float amount, float level) override;
	virtual void DamageCheck() override;
	void StopDamageCheckTimer();
	void DestroyOwner();
	void RespawnEnemy();
	bool IsTutorialMap();

	UPROPERTY(EditDefaultsOnly, Category = "HealthPotion")
	TSubclassOf<AHealthPotion> HealthPotionBlueprint;

	UPROPERTY(EditDefaultsOnly, Category = "HealthPotion")
	TSubclassOf<AEnemyBase> TutorialGoblinBlueprint;
	FTimerHandle RespawnTimerHandle;
	FVector SavedLocation;
	FRotator SavedRotation;

private:
	
	//FTimerHandle DamageCheckTimer;
	//float PendingDamage;
	//void DamageCheck();

};
