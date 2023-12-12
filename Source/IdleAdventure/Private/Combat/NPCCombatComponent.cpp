


#include "Combat/NPCCombatComponent.h"
#include <Character/Enemy_Goblin.h>
#include <PlayerEquipment/BonusManager.h>
#include <Kismet/GameplayStatics.h>
#include <Character/IdleCharacter.h>
#include <Player/IdlePlayerController.h>
#include <AbilitySystem/IdleAttributeSet.h>
#include <Player/IdlePlayerState.h>
#include <Game/SpawnManager.h>

void UNPCCombatComponent::TakeDamage(float amount)
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

void UNPCCombatComponent::DamageCheck()
{
    // Roll a random number to decide if damage is taken
    if (FMath::RandBool()) // 50% chance, adjust as needed
    {
        Health -= PendingDamage;
        if (Health <= 0)
        {
            HandleDeath();
        }

        ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
        if (OwningCharacter)
        {
            ShowDamageNumber(PendingDamage, OwningCharacter, FLinearColor::Red);
        }

        OnHealthChanged.Broadcast(Health, MaxHealth);
        //UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);
        //UE_LOG(LogTemp, Warning, TEXT("MaxHealth: %f"), MaxHealth);
    }
    else
    {
        ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
        if (OwningCharacter)
        {
            ShowDamageNumber(0, OwningCharacter, FLinearColor::White);
        }
    }
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
    IdleAttributeSet->SetWoodcutExp(IdleAttributeSet->GetWoodcutExp() + 200.0f);
    IdleAttributeSet->SetWeeklyWoodcutExp(IdleAttributeSet->GetWeeklyWoodcutExp() + 200.0f);
    Character->ShowExpNumber(200.0f, Character, FLinearColor::White);

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
        OwningCharacter->EnemyDeathAnimation();
    }

    // Delay the destruction of the owner
    if (GetWorld())
    {
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UNPCCombatComponent::DestroyOwner, 1.0f, false);
    }
}
