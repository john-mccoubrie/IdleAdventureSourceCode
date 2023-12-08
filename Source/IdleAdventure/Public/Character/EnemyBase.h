

#pragma once

#include "CoreMinimal.h"
#include "Combat/NPCCombatComponent.h"
#include "Actor/Base_NPCActor.h"
#include "NiagaraComponent.h"
#include "AI/NPCAIController.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"


class UBehaviorTree;

UCLASS(Blueprintable, BlueprintType)
class IDLEADVENTURE_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

	virtual void Interact();
	virtual void EndCombatEffects();
	virtual void EquipWeapon();

	UFUNCTION(BlueprintCallable, Category = "EnemyAttacks")
	virtual void SpawnEnemyAttackEffect();

	UFUNCTION(BlueprintCallable, Category = "Enemy Attacks")
	virtual void EnemyAttacksPlayer();

	UFUNCTION(BlueprintCallable, Category = "Enemy Attacks")
	virtual void UpdateWalkSpeed(float NewSpeed);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UBaseCombatComponent* CombatComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TSubclassOf<UBehaviorTree> BehaviorTree;

	// Use TSubclassOf to specify the AI Controller class
	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<ANPCAIController> NPCAIControllerClass;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EnemyAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* EnemyAttackEffect;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EnemyDeathMontage;

	UNiagaraComponent* SpawnedEnemyAttackEffect;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> EnemyWeapon;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
