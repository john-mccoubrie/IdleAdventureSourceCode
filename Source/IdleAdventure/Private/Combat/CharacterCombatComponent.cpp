


#include "Combat/CharacterCombatComponent.h"
#include <PlayerEquipment/BonusManager.h>
#include <Combat/CombatManager.h>

void UCharacterCombatComponent::HandleDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Player died!"));
    ACombatManager* CombatManager = ACombatManager::GetInstance(GetWorld());
    CombatManager->OnCharacterDeath.Broadcast();
}

void UCharacterCombatComponent::TakeDamage(float amount)
{
    ABonusManager* BonusManager = ABonusManager::GetInstance(GetWorld());
 
    if (FMath::RandBool())
    {
        Health -= amount - BonusManager->MultiplierSet.Armor;
        if (Health <= 0)
        {
            //Handle death
            HandleDeath();
        }
        ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
        if (OwningCharacter)
        {
            ShowDamageNumber(amount - BonusManager->MultiplierSet.Armor, OwningCharacter, FLinearColor::Red);
        }

        OnHealthChanged.Broadcast(Health, MaxHealth);
    }
    else
    {
        ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
        ShowDamageNumber(0, OwningCharacter, FLinearColor::White);
    }
    
}

void UCharacterCombatComponent::AddHealth(float HealthToAdd)
{
    if (Health < 500)
    {
        Health += HealthToAdd;
        ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
        ShowDamageNumber(HealthToAdd, OwningCharacter, FLinearColor::Green);
    }  
}

