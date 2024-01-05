

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/Class.h"
//#include "UI/DamageTextComponent.h"
#include "BaseCombatComponent.generated.h"


class UDamageTextComponent;
class UStoicTypeIndicatorComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedDelegate, float, NewHealth, float, MaxHealth);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IDLEADVENTURE_API UBaseCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBaseCombatComponent();

	void PerformAttack();
	virtual void TakeDamage(float amount, float level);
	void IsAlive();
	virtual void HandleDeath();
	virtual void AddHealth(float HealthToAdd);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, FSlateColor Color);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ChangeStoicImage(FString StoicType, ACharacter* TargetCharacter);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float AttackRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float Level;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float BossIconOffset;

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnHealthChangedDelegate OnHealthChanged;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UStoicTypeIndicatorComponent> StoicTypeComponentClass;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	
};
