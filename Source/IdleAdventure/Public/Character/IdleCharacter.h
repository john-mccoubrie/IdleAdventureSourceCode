
#pragma once

#include "CoreMinimal.h"
#include <GameFramework/SpringArmComponent.h>
#include "Quest/Quest.h"
#include <Camera/CameraComponent.h>
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

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* CofferMontage;
	//void BeginWoodcuttingAnim();

		USpringArmComponent* SpringArm;
		UCameraComponent* Camera;

		UPROPERTY(BlueprintReadWrite, Category = "Quests")
		TArray<UQuest*> ActiveQuests;

		void CheckQuestCompletionFromPlayFab(UQuest* Quest);

		UFUNCTION(BlueprintCallable, Category = "Quests")
		void AddQuest(UQuest* Quest);

		UFUNCTION(BlueprintCallable, Category = "Quests")
		void CompleteQuest(UQuest* Quest);

		UFUNCTION(BlueprintCallable, Category = "Quests")
		void UpdateQuestLogUI();
		
		UQuest* GetCurrentActiveQuest();

		void UpdateAllActiveQuests(const FString& ObjectiveType, int32 Amount);

		void NotifyQuestCompletionStatus(FString QuestID, bool bCanAccept);

		bool HasQuestWithVersion(const FString& QuestID, const FString& Version) const;

private:
	void InitAbilityActorInfo();
	
	
};
