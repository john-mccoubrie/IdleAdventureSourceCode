


#include "Character/Enemy_Demon_Boss.h"
#include "Combat/DemonBossCombatComponent.h"
#include "Combat/CombatManager.h"
#include <Kismet/GameplayStatics.h>
#include <Character/IdleCharacter.h>

AEnemy_Demon_Boss::AEnemy_Demon_Boss()
{
	DemonBossCombatComponent = CreateDefaultSubobject<UDemonBossCombatComponent>(TEXT("DemonBossCombatComponent"));
}

void AEnemy_Demon_Boss::Interact()
{
	//Super::Interact();
	
	UE_LOG(LogTemp, Warning, TEXT("Enemy demon boss interact called"));
	ACombatManager* CombatManager = ACombatManager::GetInstance(GetWorld());
	AIdleCharacter* PlayerCharacter = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	CombatManager->HandleCombat(PlayerCharacter->CombatComponent, DemonBossCombatComponent, 0);
	
}


void AEnemy_Demon_Boss::SpawnEnemyAttackEffect()
{
    if (DemonBossCombatComponent && DemonBossCombatComponent->bIsTailspinActive)
    {
        // If tailspin is active, don't handle any standard combat here
        UE_LOG(LogTemp, Warning, TEXT("Tailspin attack is active, skipping standard combat"));
        return;
    }
	Super::SpawnEnemyAttackEffect();
}

void AEnemy_Demon_Boss::EndCombatEffects()
{
	Super::EndCombatEffects();
}

void AEnemy_Demon_Boss::EnemyAttacksPlayer()
{
    // Check if the tailspin attack is active
    if (DemonBossCombatComponent && DemonBossCombatComponent->bIsTailspinActive)
    {
        // If tailspin is active, don't handle any standard combat here
        UE_LOG(LogTemp, Warning, TEXT("Tailspin attack is active, skipping standard combat"));
        return;
    }

    // If tailspin is not active, proceed with standard combat
    UE_LOG(LogTemp, Warning, TEXT("Demon boss attacks player"));
    ACombatManager* CombatManager = ACombatManager::GetInstance(GetWorld());
    AIdleCharacter* PlayerCharacter = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (PlayerCharacter)
    {
        CombatManager->HandleCombat(DemonBossCombatComponent, PlayerCharacter->CombatComponent, 10.0f);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Player character not found for combat"));
    }

    // Initialize tailspin attack timer if not already set
    if (!bDemonIsTimerSet)
    {
        DemonBossCombatComponent->InitializeTailspinAttackTimer();
        bDemonIsTimerSet = true;
    }
}
