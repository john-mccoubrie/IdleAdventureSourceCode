


#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "Quest/Quest.h"
#include "AbilitySystem/Abilities/IdleGameplayAbility.h"
#include "WoodcuttingAbility.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTreeLifespanChanged, float, TotalDuration);

class AbilitySystemComponent;
class GameplayEffect;

/**
 *
 */
UCLASS()
class IDLEADVENTURE_API UWoodcuttingAbility : public UGameplayAbility
{
	GENERATED_BODY()

	UWoodcuttingAbility();
	~UWoodcuttingAbility();

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnTreeCutDown();

	UFUNCTION()
	void SetDuration(float TotalDuration);

	UFUNCTION()
	void AddEssenceToInventory();

	void CalculateLogYield(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecExecuted, FActiveGameplayEffectHandle ActiveHandle);
	void AddExperience(float Amount);
	void GetLegendaryEssence();
	bool bIsChoppingLegendaryTree;
	void CallEndAbility();

	void GainExperience(const FGameplayAbilityActorInfo* ActorInfo);
	void OnGameplayEffectRemoved(const FActiveGameplayEffect& Effect);

	float WisdomThreshold;
	float TemperanceThreshold;
	float JusticeThreshold;
	float CourageThreshold;

	FActiveGameplayEffectHandle GetActiveEffectHandle() const;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FTreeLifespanChanged OnTreeLifespanChanged;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	static int32 InstanceCounter;

	FTimerHandle StackingTimerHandle;

	void StopCutDownTimer();
	FTimerHandle StopWoodcuttingTimerHandle;
	FActiveGameplayEffectHandle ActiveEffectHandle;

	float ExperienceGain;

	bool bAbilityIsActive;

	UQuest* CurrentQuest = nullptr;


private:



};
