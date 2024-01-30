


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
    AEnemyBase* OwningCharacter = Cast<AEnemyBase>(GetOwner());
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
