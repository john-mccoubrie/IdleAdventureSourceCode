

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Inventory.h"
#include "IdleCharacterBase.h"
#include "IdleCharacter.generated.h"



class UAnimMontage;
class UAttributeSet;
class UAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API AIdleCharacter : public AIdleCharacterBase
{
	GENERATED_BODY()

public:
	AIdleCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void BeginPlay() override;

	// Player's inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	AInventory* CharacterInventory;

	UPROPERTY(EditAnywhere, Category = "Inventory")
		float EssenceCount;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
		UAnimMontage* WoodcutMontage;
	//void BeginWoodcuttingAnim();

		

private:
	void InitAbilityActorInfo();
	
	
};
