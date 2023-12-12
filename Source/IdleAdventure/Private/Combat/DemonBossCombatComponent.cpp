


#include "Combat/DemonBossCombatComponent.h"
#include <Player/IdlePlayerController.h>
#include <Player/IdlePlayerState.h>
#include <Character/IdleCharacter.h>
#include <AbilitySystem/IdleAttributeSet.h>
#include <PlayerEquipment/BonusManager.h>
#include <Kismet/GameplayStatics.h>
#include <Game/SpawnManager.h>

void UDemonBossCombatComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UDemonBossCombatComponent::HandleDeath()
{
    Super::HandleDeath();

    ASpawnManager* SpawnManager = ASpawnManager::GetInstance(GetWorld());
    SpawnManager->UpdateBossCount(1);

    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();

    //Update quest
    ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
    AIdleCharacter* Character = Cast<AIdleCharacter>(MyCharacter);
    Character->UpdateAllActiveQuests("BossKills", 1);

    //Handle Exp
    UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);
    IdleAttributeSet->SetWoodcutExp(IdleAttributeSet->GetWoodcutExp() + 5000.0f);
    IdleAttributeSet->SetWeeklyWoodcutExp(IdleAttributeSet->GetWeeklyWoodcutExp() + 5000.0f);
    Character->ShowExpNumber(5000.0f, Character, FLinearColor::White);

    //Update player controller values
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

void UDemonBossCombatComponent::TakeDamage(float amount)
{
    //Super::TakeDamage(amount);
	ABonusManager* bonusManager = ABonusManager::GetInstance(GetWorld());

	//InitializeDamagingCircleTimer();

	// Retrieve and apply damage bonuses
	PendingDamage = amount + bonusManager->MultiplierSet.Damage;

	if (!GetWorld()->GetTimerManager().IsTimerActive(DamageCheckTimer))
	{
		// Start a repeating timer that checks for damage every second
		GetWorld()->GetTimerManager().SetTimer(DamageCheckTimer, this, &UDemonBossCombatComponent::DamageCheck, 1.0f, true);
	}
}

void UDemonBossCombatComponent::StopDamageCheckTimer()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(DamageCheckTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(DamageCheckTimer);
	}
}

void UDemonBossCombatComponent::DamageCheck()
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
