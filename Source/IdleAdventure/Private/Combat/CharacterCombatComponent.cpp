


#include "Combat/CharacterCombatComponent.h"
#include <PlayerEquipment/BonusManager.h>
#include <Combat/CombatManager.h>
#include <Game/SpawnManager.h>
#include <Player/IdlePlayerController.h>
#include <Player/IdlePlayerState.h>
#include <Game/SteamManager.h>
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
    if (SpawnManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("Map Difficulty at Death: %s"), *SpawnManager->RewardsToSend.MapDifficulty);

        int32 TotalSeconds = static_cast<int32>(SpawnManager->CountdownTime);
        int32 Minutes = TotalSeconds / 60;
        int32 Seconds = TotalSeconds % 60;

        FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
        CombatManager->OnCharacterDeath.Broadcast(TimeString, SpawnManager->RewardsToSend.MapDifficulty, "Player Death",
            "Visit the StoicStore to purchase capes and staves to increase your armor, attack, and overall health. Killing enemies has a 50% chance to drop health potions and adding to the legendary meters grants 20 health."
        );

        SpawnManager->StopCountdownTimer();
    }

    ASteamManager* SteamManager = ASteamManager::GetInstance(GetWorld());
    if (SteamManager)
    {
        SteamManager->UnlockSteamAchievement(TEXT("MOMENTO_MORI"));
    }
}

void UCharacterCombatComponent::TakeDamage(float amount, float enemyLevel)
{
    ABonusManager* BonusManager = ABonusManager::GetInstance(GetWorld());

    // Getting player's level from the player state
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);
    float playerLevel = IdleAttributeSet->GetWoodcuttingLevel(); // Replace with actual combat level attribute

    // Define scaling factors
    float ScalingFactorHigher = 0.05; // Adjust as needed
    float ScalingFactorLower = -0.1; // Adjust as needed

    // Adjust hit probability based on player and enemy levels with distinction
    float levelDifference = playerLevel - enemyLevel;
    float adjustedLevelDifference = (levelDifference > 0) ? levelDifference * ScalingFactorHigher : levelDifference * ScalingFactorLower;
    float hitProbability = FMath::Clamp(0.5f + adjustedLevelDifference, 0.1f, 0.9f); // Hit probability between 10% and 90%

    // Random roll to decide if the hit lands
    if (FMath::RandRange(0.0f, 1.0f) <= hitProbability)
    {
        // Skew the damage range towards lower values
        float baseDamage = amount; // Base damage is the amount parameter
        float armorEffect = BonusManager->MultiplierSet.Armor;
        float effectiveDamage = baseDamage - armorEffect; // Damage reduced by armor

        // Ensure effective damage isn't less than a minimum value
        float minDamage = FMath::Max(1.0f, effectiveDamage * 0.1f); // Minimum damage is at least 1 or 10% of effective damage
        float maxDamage = FMath::Max(effectiveDamage, minDamage); // Max damage is the higher of effective damage or minDamage

        // Generate damage
        float finalDamage = FMath::RandRange(minDamage, maxDamage);

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

        // Play player take hit sound
        PC->IdleInteractionComponent->PlayPlayerTakeHitSound();
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
    if (Health <= 500)
    {
        Health += HealthToAdd;
        ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
        ShowDamageNumber(HealthToAdd, OwningCharacter, FLinearColor::Green);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Health is over 500"));
    }
}

