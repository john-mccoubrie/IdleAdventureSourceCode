


#include "Combat/NPCCombatComponent.h"
#include <Character/Enemy_Goblin.h>
#include <PlayerEquipment/BonusManager.h>
#include <Kismet/GameplayStatics.h>
#include <Character/IdleCharacter.h>
#include <Player/IdlePlayerController.h>

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

    //Update Quest
    ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
    AIdleCharacter* Character = Cast<AIdleCharacter>(MyCharacter);
    Character->UpdateAllActiveQuests("EnemyKills", 1);

    //Handle Exp

    //Update player controller values
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    PC->InteruptCombat();

    // Stop the damage check timer
    if (GetWorld()->GetTimerManager().IsTimerActive(DamageCheckTimer))
    {
        GetWorld()->GetTimerManager().ClearTimer(DamageCheckTimer);
    }

    //Destroy the enemy actor
    if (AActor* Owner = GetOwner())
    {
        Owner->Destroy();
    }
}
