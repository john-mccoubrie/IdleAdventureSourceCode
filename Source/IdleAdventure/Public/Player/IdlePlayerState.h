#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include <AbilitySystem/Abilities/ConversionAbility.h>
#include "GameplayEffectTypes.h"
#include "Actor/IdleEffectActor.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "IdlePlayerState.generated.h"


class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayAbility;
//class ULeaderboardManager;
class UWoodcuttingAbility;
class UConversionAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEXPChanged, float, NewEXP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWhenLevelUp, float, CurrentExp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FShowExpNumbersOnText, float, CurrentExp, float, MaxExp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShowPlayerLevelOnText, float, CurrentPlayerLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExpGained, float, OnExpGained);

// Struct to define a gameplay ability information
USTRUCT(BlueprintType)
struct FGameplayAbilityInfo
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay Ability Info")
		TSubclassOf<UGameplayAbility> Ability;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay Ability Info")
		int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay Ability Info")
		int32 InputID;

		FGameplayAbilityInfo()
			: Level(0)
			, InputID(0)

		{

		}
};



UCLASS()
class IDLEADVENTURE_API AIdlePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AIdlePlayerState();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	UAbilitySystemComponent* GetAbilitySystemComponent() const;

	//virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	UAbilitySystemComponent* AbilitySystemComponent;

	//UPROPERTY()
		//TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
		TObjectPtr<UAttributeSet> AttributeSet;

	//UPROPERTY(EditAnywhere)
		//TObjectPtr<ULeaderboardManager> LeaderboardManager;

		TMap<TSubclassOf<UGameplayAbility>, FGameplayAbilitySpecHandle> AbilityHandles;

		void GiveAbility(const FGameplayAbilityInfo& AbilityInfo);
		void ActivateAbility(TSubclassOf<UGameplayAbility> AbilityToActivate);
		void DeactivateAbility(TSubclassOf<UGameplayAbility> AbilityToDeactivate);
		bool IsAbilityActive(TSubclassOf<UGameplayAbility> AbilityClass);

	/** Checks if the player should level up and handles it */

	UFUNCTION(BlueprintCallable, Category = "Initialization")
	void InitializePlayerValues();

	//FOnAttributeChangeData does not work with UFUNCTION()
	void CheckForLevelUp(const FOnAttributeChangeData& Data) const;

	/** Adjusts the XPToNextLevel based on your progression curve */
	void UpdateXPThreshold();

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
		FOnEXPChanged OnEXPChanged;

		UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
		FWhenLevelUp WhenLevelUp;

		UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
		FShowExpNumbersOnText ShowExpNumbersOnText;

		UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
		FShowPlayerLevelOnText ShowPlayerLevelOnText;

		UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
		FExpGained OnExpGained;

	//UPROPERTY()
		//UWoodcuttingAbility* WoodcuttingAbility;

	UPROPERTY()
		UConversionAbility* ConversionAbility;

		UPROPERTY()
		TArray<ACoffer*> ActiveCoffers;

		FTimerHandle SaveGameTimerHandle;

		void AutoSaveGame();
		void LoadExp();
		UFUNCTION()
		void OnPlayFabExpLoaded(int32 LoadedExp);
		UFUNCTION()
		void OnPlayFabWeeklyExpLoaded(int32 LoadedWeeklyExp);
		UFUNCTION()
		void OnPlayFabPlayerLevelLoaded(int32 LoadedPlayerLevel);

		UPROPERTY(EditAnywhere, Category = "Tree")
		TSubclassOf<AIdleEffectActor> TreeBlueprintClass;


private:

	UPROPERTY()
		TArray<FGameplayAbilityInfo> GrantedAbilities;

		bool bHasBoundToPlayFabDelegate = false;
		int32 LocalWoodcuttingExp = 0;
		int32 LocalPlayerLevel = 0;
		int32 LocalWeeklyWoodcuttingExp = 0;
};
