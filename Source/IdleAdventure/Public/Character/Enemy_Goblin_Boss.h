

#pragma once

#include "CoreMinimal.h"
#include "Combat/GoblinBossCombatComponent.h"
#include "Character/EnemyBase.h"
#include "Enemy_Goblin_Boss.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API AEnemy_Goblin_Boss : public AEnemyBase
{
	GENERATED_BODY()

public:

	AEnemy_Goblin_Boss();
	virtual void Interact() override;
	virtual void SpawnEnemyAttackEffect() override;
	virtual void EndCombatEffects() override;
	virtual void EnemyAttacksPlayer() override;


	UPROPERTY()
	UGoblinBossCombatComponent* BossCombatComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GoblinBossWeapon")
	USkeletalMeshComponent* GoblinBossWeapon;

private:
	bool bIsTimerSet = false;
};
