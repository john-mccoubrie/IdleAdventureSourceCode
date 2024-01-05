

#pragma once

#include "CoreMinimal.h"
#include "Character/EnemyBase.h"
#include "Enemy_Ghoul.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API AEnemy_Ghoul : public AEnemyBase
{
	GENERATED_BODY()
	AEnemy_Ghoul();
	virtual void Interact() override;
	virtual void SpawnEnemyAttackEffect() override;
	virtual void EndCombatEffects() override;
	virtual void EnemyAttacksPlayer() override;
	virtual void EnemyDeathAnimation() override;
	virtual void UpdateWalkSpeed(float NewSpeed) override;
	
};
