


#include "AbilitySystem/Abilities/CombatAbility.h"
#include <Character/IdleCharacter.h>

void UCombatAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    AIdleCharacter* Character = Cast<AIdleCharacter>(ActorInfo->AvatarActor.Get());
    UAnimMontage* AnimMontage = Character->WoodcutMontage;
    Character->PlayAnimMontage(AnimMontage);
}
