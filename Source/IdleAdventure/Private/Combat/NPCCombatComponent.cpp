


#include "Combat/NPCCombatComponent.h"
#include <PlayerEquipment/BonusManager.h>
#include <Kismet/GameplayStatics.h>
#include <Character/IdleCharacter.h>
#include <Player/IdlePlayerController.h>
#include <AbilitySystem/IdleAttributeSet.h>
#include <Player/IdlePlayerState.h>
#include <Game/SpawnManager.h>

void UNPCCombatComponent::TakeDamage(float amount, float level)
{
    UE_LOG(LogTemp, Warning, TEXT("Take damage called in NPC combat component"));
    ABonusManager* bonusManager = ABonusManager::GetInstance(GetWorld());

    // Retrieve and apply damage bonuses
    PendingDamage = amount + bonusManager->MultiplierSet.Damage;

    if (!GetWorld()->GetTimerManager().IsTimerActive(DamageCheckTimer))
    {
        // Start a repeating timer that checks for damage every second
        GetWorld()->GetTimerManager().SetTimer(DamageCheckTimer, this, &UNPCCombatComponent::DamageCheck, 1.0f, true);
    }
}

void UNPCCombatComponent::DamageCheck()
{
    Super::DamageCheck();
}

void UNPCCombatComponent::StopDamageCheckTimer()
{
    if (GetWorld()->GetTimerManager().IsTimerActive(DamageCheckTimer))
    {
        GetWorld()->GetTimerManager().ClearTimer(DamageCheckTimer);
    }
}

void UNPCCombatComponent::DestroyOwner()
{
    if (AActor* Owner = GetOwner())
    {
        Owner->Destroy();
    }
}

void UNPCCombatComponent::RespawnEnemy()
{
    UWorld* World = GetWorld();
    if (World)
    {
        UE_LOG(LogTemp, Warning, TEXT("Made it to respawn enemy"));
        // Spawn the health potion using the Blueprint reference
        AEnemy_Goblin* SpawnedGoblin = World->SpawnActor<AEnemy_Goblin>(TutorialGoblinBlueprint, SavedLocation, SavedRotation);
        if (SpawnedGoblin)
        {
            UE_LOG(LogTemp, Warning, TEXT("Spawned tutorial goblin"));
        }
    }
}

bool UNPCCombatComponent::IsTutorialMap()
{
    if (GetWorld())
    {
        // Get the current level name
        FString LevelName = GetWorld()->GetMapName();
        //LevelName = UGameplayStatics::RemoveLevelPrefix(LevelName); // Optional: Clean the level name

        // Log the level name for debugging
        UE_LOG(LogTemp, Warning, TEXT("Current level name: %s"), *LevelName);

        // Check if it's the TutorialMap
        return LevelName.Equals("TutorialMap", ESearchCase::IgnoreCase);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GetWorld() returned nullptr, unable to determine level name."));
    }

    return false;
}

/*
void UNPCCombatComponent::DamageCheck()
{
    // Getting player's level from the player state
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);
    float playerLevel = IdleAttributeSet->GetWoodcuttingLevel(); // Replace with actual combat level attribute
    float goblinLevel = this->Level; // Goblin's level

    float baseDamage = PendingDamage; // Base damage

    // Calculate hit probability based on player and goblin levels
    float levelRatio = playerLevel / goblinLevel;
    float hitProbability = FMath::Clamp(levelRatio * 0.5f, 0.0f, 1.0f); // Adjust for higher miss chance

    // Random roll to decide if the hit lands
    if (FMath::RandRange(0.0f, 1.0f) <= hitProbability)
    {
        // Skew the damage range towards lower values
        float minDamage = 2.0f; // Fixed minimum
        float maxDamage = baseDamage; // Max damage remains as base damage

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
*/

void UNPCCombatComponent::HandleDeath()
{
    Super::HandleDeath();


    ASpawnManager* SpawnManager = ASpawnManager::GetInstance(GetWorld());
    SpawnManager->UpdateEnemyCount(1);

    //Update Quest
    ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
    AIdleCharacter* Character = Cast<AIdleCharacter>(MyCharacter);
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    Character->UpdateAllActiveQuests("EnemyKills", 1);

    //Handle health potion drop
    if (FMath::RandBool() && HealthPotionBlueprint)
    {
        AActor* Owner = GetOwner();
        SavedLocation = Owner->GetActorLocation();
        SavedRotation = Owner->GetActorRotation();
        FVector SpawnLocation = Owner ? Owner->GetActorLocation() : FVector::ZeroVector;
        FRotator SpawnRotation = Owner ? Owner->GetActorRotation() : FRotator::ZeroRotator;

        UWorld* World = GetWorld();
        if (World)
        {
            // Spawn the health potion using the Blueprint reference
            AHealthPotion* SpawnedPotion = World->SpawnActor<AHealthPotion>(HealthPotionBlueprint, SpawnLocation, SpawnRotation);
            if (SpawnedPotion)
            {
                UE_LOG(LogTemp, Warning, TEXT("Enemy dropped health potion"));
                // Additional code for the spawned potion if necessary
            }
        }
    }

    //Handle Exp
    UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);
    IdleAttributeSet->SetWoodcutExp(IdleAttributeSet->GetWoodcutExp() + Experience);
    IdleAttributeSet->SetWeeklyWoodcutExp(IdleAttributeSet->GetWeeklyWoodcutExp() + Experience);
    Character->ShowExpNumber(Experience, Character, FLinearColor::White);

    //Update player controller values
    PC->InteruptCombat();

    // Stop the damage check timer
    if (GetWorld()->GetTimerManager().IsTimerActive(DamageCheckTimer))
    {
        GetWorld()->GetTimerManager().ClearTimer(DamageCheckTimer);
    }

    //Handle death animation
    AEnemy_Goblin* OwningCharacter = Cast<AEnemy_Goblin>(GetOwner());
    if (OwningCharacter)
    {
        OwningCharacter->EndCombatEffects();
        OwningCharacter->EnemyDeathAnimation();
    }

    // Delay the destruction of the owner
    if (GetWorld())
    {
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UNPCCombatComponent::DestroyOwner, 1.0f, false);
    }
    if (IsTutorialMap())
    {
        UE_LOG(LogTemp, Warning, TEXT("Countdown started for enemy respawn"));
        //ASpawnManager* SpawnManager = ASpawnManager::GetInstance(GetWorld());
        SpawnManager->ScheduleRespawn("Goblin", TutorialGoblinBlueprint, SavedLocation, SavedRotation);
        //GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &UNPCCombatComponent::RespawnEnemy, 5.0f, false);
    }
}
