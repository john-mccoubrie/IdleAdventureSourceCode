

#pragma once

#include "CoreMinimal.h"
#include "Character/EnemyBase.h"
#include "Enemy_Demon.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API AEnemy_Demon : public AEnemyBase
{
	GENERATED_BODY()
	AEnemy_Demon();
	virtual void Interact() override;
	virtual void EquipWeapon() override;
	virtual void SpawnEnemyAttackEffect() override;
	virtual void EndCombatEffects() override;
	virtual void EnemyAttacksPlayer() override;
	virtual void UpdateWalkSpeed(float NewSpeed) override;
};
