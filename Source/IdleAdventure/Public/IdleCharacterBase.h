#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WoodcuttingAbility.h"
#include "Interact/WoodcuttingInterface.h"
#include "GameFramework/Character.h"
#include "IdleCharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayAbility;
class UGameplayEffect;
class UWoodcuttingAbility;

UCLASS(Abstract)
class IDLEADVENTURE_API AIdleCharacterBase : public ACharacter, public IAbilitySystemInterface, public IWoodcuttingInterface
{
	GENERATED_BODY()

public:
	AIdleCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UPROPERTY(EditAnywhere, Category = "Abilities")
		TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

		

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere)
		TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
		TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
		TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	void InitializePrimaryAttributes() const;


	void AddCharacterAbilities();

	

	
};
