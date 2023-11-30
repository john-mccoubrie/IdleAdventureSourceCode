

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseCombatComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChangedDelegate, UBaseCombatComponent*, CombatComponent, float, NewHealth, float, MaxHealth);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IDLEADVENTURE_API UBaseCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBaseCombatComponent();

	void PerformAttack();
	void TakeDamage(float amount);
	void IsAlive();
	virtual void HandleDeath();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float AttackRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float Damage;

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnHealthChangedDelegate OnHealthChanged;
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	
};
