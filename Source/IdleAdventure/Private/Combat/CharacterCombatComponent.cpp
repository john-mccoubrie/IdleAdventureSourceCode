


#include "Combat/CharacterCombatComponent.h"
#include <PlayerEquipment/BonusManager.h>
#include <Combat/CombatManager.h>
#include <Game/SpawnManager.h>
#include <Player/IdlePlayerController.h>
#include <Player/IdlePlayerState.h>
#include <AbilitySystem/IdleAttributeSet.h>

void UCharacterCombatComponent::HandleDeath()
{

    //if current level == tutorial, reset the player position
    //interupt combat
    //stop enemy timer if active
    //stop boss timer if active
    //reset player position to initial spawn point

	UE_LOG(LogTemp, Warning, TEXT("Player died!"));
    ACombatManager* CombatManager = ACombatManager::GetInstance(GetWorld());
    ASpawnManager* SpawnManager = ASpawnManager::GetInstance(GetWorld());
    int32 TotalSeconds = static_cast<int32>(SpawnManager->CountdownTime);
    int32 Minutes = TotalSeconds / 60;
    int32 Seconds = TotalSeconds % 60;

    FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
    CombatManager->OnCharacterDeath.Broadcast(TimeString, SpawnManager->RewardsToSend.MapDifficulty, "Player Death", 
        "Visit the StoicStore to purchase capes and staves to increase your armor, attack, and overall health. Killing enemies has a 50% chance to drop health potions and adding to the legendary meters grants 20 health."
    );
}

void UCharacterCombatComponent::TakeDamage(float amount, float enemyLevel)
{
    ABonusManager* BonusManager = ABonusManager::GetInstance(GetWorld());

    // Getting player's level from the player state
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);
    float playerLevel = IdleAttributeSet->GetWoodcuttingLevel(); // Replace with actual combat level attribute

    // Calculate hit probability based on enemy and player levels
    float levelDifference = enemyLevel - playerLevel;
    float hitProbability = FMath::Clamp(0.5f + (levelDifference / 10.0f), 0.0f, 1.0f); // Adjusted for more balanced hit chance

    // Random roll to decide if the hit lands
    if (FMath::RandRange(0.0f, 1.0f) <= hitProbability)
    {
        // Skew the damage range towards lower values
        float baseDamage = amount; // Base damage is the amount parameter
        float armorEffect = BonusManager->MultiplierSet.Armor;
        float effectiveDamage = baseDamage - armorEffect; // Damage reduced by armor

        float minDamage = 2.0f; // Fixed minimum
        float maxDamage = FMath::Max(effectiveDamage, minDamage); // Ensure max damage isn't lower than min

        // Generate two random numbers and use the lower one for more frequent low damage hits
        float randomDamage1 = FMath::RandRange(minDamage, maxDamage);
        float randomDamage2 = FMath::RandRange(minDamage, maxDamage);
        float finalDamage = FMath::Min(randomDamage1, randomDamage2);

        // Round to nearest whole number
        int32 finalDamageInt = FMath::RoundToInt(finalDamage);
        finalDamage = static_cast<float>(finalDamageInt);

        // Apply damage
        Health -= finalDamage;
        if (Health <= 0)
        {
            // Handle death
            HandleDeath();
        }

        // Show damage number (red for damage, white for miss)
        ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
        if (OwningCharacter)
        {
            ShowDamageNumber(finalDamage, OwningCharacter, FLinearColor::Red);
        }
    }
    else
    {
        // Missed attack
        ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
        if (OwningCharacter)
        {
            ShowDamageNumber(0, OwningCharacter, FLinearColor::White);
        }
    }

    // Broadcast health change
    OnHealthChanged.Broadcast(Health, MaxHealth);
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

