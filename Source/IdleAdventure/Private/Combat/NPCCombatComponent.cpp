


#include "Combat/NPCCombatComponent.h"
#include <Character/Enemy_Goblin.h>

void UNPCCombatComponent::HandleDeath()
{
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("OwnerActor is nullptr."));
        return;
    }

    AEnemy_Goblin* Goblin = Cast<AEnemy_Goblin>(OwnerActor);
    if (!Goblin)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cast to AEnemy_Goblin failed."));
        return;
    }

    /*
    if (Goblin->IsDead())  // Make sure Goblin has a method IsDead() to check the state
    {
        // Goblin is already dead, so don't play the death animation again
        return;
    }

    if (Goblin->IsAnyMontagePlaying())
    {
        // Decide how to handle if an animation is already playing. For now, just returning.
        return;
    }
    */

    UAnimMontage* AnimMontage = Goblin->EnemyDeathMontage;
    if (!AnimMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("AnimMontage is nullptr."));
        return;
    }

    Goblin->PlayAnimMontage(AnimMontage);
    UE_LOG(LogTemp, Warning, TEXT("Goblin died!"));
}
