


#include "AbilitySystem/Abilities/IdleGameplayAbility.h"
#include "Player/IdlePlayerState.h"
#include "GameplayEffect.h"
#include "AbilitySystem/IdleAttributeSet.h"
#include "GameplayEffectExtension.h"
#include <Player/IdlePlayerController.h>
#include "UI/WidgetController/OverlayWidgetController.h"
#include <AbilitySystemBlueprintLibrary.h>

UIdleGameplayAbility::UIdleGameplayAbility()
{
    //UE_LOG(LogTemp, Warning, TEXT("UIdleGameplayAbility"));
}

void UIdleGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    UE_LOG(LogTemp, Warning, TEXT("UIdleGameplayAbility ActivateAbility"));
}