

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Character/EnemyBase.h"
#include "Enemy_Goblin.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API AEnemy_Goblin : public AEnemyBase
{
	GENERATED_BODY()
public:

	virtual void Interact() override;

	UFUNCTION(BlueprintCallable, Category = "EnemyAttacks")
	void SpawnEnemyAttackEffect();
	
	virtual void EndCombatEffects() override;

	UFUNCTION(BlueprintCallable, Category = "Enemy Attacks")
	void EnemyAttacksPlayer();

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EnemyAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* EnemyAttackEffect;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EnemyDeathMontage;

	UNiagaraComponent* SpawnedEnemyAttackEffect;
};
