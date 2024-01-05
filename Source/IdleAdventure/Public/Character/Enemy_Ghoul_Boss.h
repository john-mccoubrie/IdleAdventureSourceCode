

#pragma once

#include "CoreMinimal.h"
#include <Combat/GhoulBossCombatComponent.h>
#include "Character/EnemyBase.h"
#include "Enemy_Ghoul_Boss.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API AEnemy_Ghoul_Boss : public AEnemyBase
{
	GENERATED_BODY()
public:
	AEnemy_Ghoul_Boss();
	virtual void Interact() override;
	virtual void SpawnEnemyAttackEffect() override;
	virtual void EndCombatEffects() override;
	virtual void EnemyAttacksPlayer() override;
	void SpawnWindUpEffect();
	void EndWindUpEffect();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Component")
	UGhoulBossCombatComponent* GhoulBossCombatComponent;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* GhoulSwipeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* GhoulWindUpEffect;

	UNiagaraComponent* SpawnedGhoulEffect;

private:
	float TestingFloat;
	bool bGhoulIsTimerSet = false;
};
