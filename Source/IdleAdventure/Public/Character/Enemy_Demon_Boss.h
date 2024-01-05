

#pragma once

#include "CoreMinimal.h"
#include <Combat/DemonBossCombatComponent.h>
#include "Character/EnemyBase.h"
#include "Enemy_Demon_Boss.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API AEnemy_Demon_Boss : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	AEnemy_Demon_Boss();
	virtual void Interact() override;
	virtual void SpawnEnemyAttackEffect() override;
	virtual void EndCombatEffects() override;
	virtual void EnemyAttacksPlayer() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Component")
	UDemonBossCombatComponent* DemonBossCombatComponent;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DemonTailspinMontage;

private:
	float TestingFloat;
	bool bDemonIsTimerSet = false;
};
