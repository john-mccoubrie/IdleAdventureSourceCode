


#include "Combat/CharacterCombatComponent.h"
#include <PlayerEquipment/BonusManager.h>

void UCharacterCombatComponent::HandleDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Player died!"));
}

void UCharacterCombatComponent::TakeDamage(float amount)
{
    Health -= amount;

    if (Health <= 0)
    {
        //Handle death
        HandleDeath();
    }
    ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
    if (OwningCharacter)
    {
        ShowDamageNumber(amount, OwningCharacter, FLinearColor::Red);
    }

    OnHealthChanged.Broadcast(Health, MaxHealth);
}

