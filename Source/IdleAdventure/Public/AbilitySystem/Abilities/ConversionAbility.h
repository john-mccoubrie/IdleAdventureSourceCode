

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Actor/Coffer.h"
#include "AbilitySystem/Abilities/IdleGameplayAbility.h"
#include "ConversionAbility.generated.h"


/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API UConversionAbility : public UIdleGameplayAbility
{
	GENERATED_BODY()
	
public:

	UConversionAbility();
	virtual ~UConversionAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void CovertEssenceToEXP();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gameplay Effects")
		TSubclassOf<UGameplayEffect> ConGameplayEffect;

		UFUNCTION()
		void OnDurationEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo, float CofferDuration, ACoffer* ClickedCoffer);

		DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEffectRemovedDelegate, const FGameplayEffectRemovalInfo&, RemovalInfo);

		UPROPERTY(BlueprintAssignable)
		FOnEffectRemovedDelegate OnEffectRemoved;

		float CoffersActive;

};
