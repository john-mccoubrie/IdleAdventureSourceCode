


#include "Character/Enemy_Ghoul_Boss.h"
#include <Combat/CombatManager.h>
#include <Character/IdleCharacter.h>
#include <Kismet/GameplayStatics.h>
#include <NiagaraFunctionLibrary.h>

AEnemy_Ghoul_Boss::AEnemy_Ghoul_Boss()
{
	GhoulBossCombatComponent = CreateDefaultSubobject<UGhoulBossCombatComponent>(TEXT("GhoulBossCombatComponent"));
}

void AEnemy_Ghoul_Boss::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy demon boss interact called"));
	ACombatManager* CombatManager = ACombatManager::GetInstance(GetWorld());
	AIdleCharacter* PlayerCharacter = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	CombatManager->HandleCombat(PlayerCharacter->CombatComponent, GhoulBossCombatComponent, 0);
}

void AEnemy_Ghoul_Boss::SpawnEnemyAttackEffect()
{
	if (GhoulBossCombatComponent && GhoulBossCombatComponent->bIsSwipeAttackActive)
	{
		// If tailspin is active, don't handle any standard combat here
		UE_LOG(LogTemp, Warning, TEXT("Swipe attack is active, skipping standard combat"));
		return;
	}
	Super::SpawnEnemyAttackEffect();
}

void AEnemy_Ghoul_Boss::EndCombatEffects()
{
	Super::EndCombatEffects();
}

void AEnemy_Ghoul_Boss::EnemyAttacksPlayer()
{
    // Check if the tailspin attack is active
    if (GhoulBossCombatComponent && GhoulBossCombatComponent->bIsSwipeAttackActive)
    {
        // If tailspin is active, don't handle any standard combat here
        UE_LOG(LogTemp, Warning, TEXT("Swipe attack is active, skipping standard combat"));
        return;
    }

    // If tailspin is not active, proceed with standard combat
    UE_LOG(LogTemp, Warning, TEXT("Ghoul boss attacks player"));
    ACombatManager* CombatManager = ACombatManager::GetInstance(GetWorld());
    AIdleCharacter* PlayerCharacter = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (PlayerCharacter)
    {
        CombatManager->HandleCombat(GhoulBossCombatComponent, PlayerCharacter->CombatComponent, 40.0f);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Player character not found for combat"));
    }

    // Initialize tailspin attack timer if not already set
    if (!bGhoulIsTimerSet)
    {
        GhoulBossCombatComponent->InitializeSwipeAttackTimer();
        bGhoulIsTimerSet = true;
    }
}

void AEnemy_Ghoul_Boss::SpawnWindUpEffect()
{
    SpawnedGhoulEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), GhoulWindUpEffect, this->GetActorLocation());
}

void AEnemy_Ghoul_Boss::EndWindUpEffect()
{
    if (SpawnedGhoulEffect)
    {
        SpawnedGhoulEffect->Deactivate();
    }
}
