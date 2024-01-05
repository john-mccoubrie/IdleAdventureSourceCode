


#include "Character/Enemy_Goblin_Boss.h"
#include <Combat/CombatManager.h>
#include <Combat/GoblinBossCombatComponent.h>

AEnemy_Goblin_Boss::AEnemy_Goblin_Boss()
{
	BossCombatComponent = CreateDefaultSubobject<UGoblinBossCombatComponent>(TEXT("GoblinBossCombatComponent"));

	GoblinBossWeapon = CreateDefaultSubobject<USkeletalMeshComponent>("GoblinBossWeapon");
	GoblinBossWeapon->SetupAttachment(GetMesh(), FName("LeftHandSocket"));
	GoblinBossWeapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy_Goblin_Boss::Interact()
{
	Super::Interact();
	UE_LOG(LogTemp, Warning, TEXT("Enemy boss interact called"));
}


void AEnemy_Goblin_Boss::SpawnEnemyAttackEffect()
{
	Super::SpawnEnemyAttackEffect();
}

void AEnemy_Goblin_Boss::EndCombatEffects()
{
	Super::EndCombatEffects();
}

void AEnemy_Goblin_Boss::EnemyAttacksPlayer()
{
	Super::EnemyAttacksPlayer();

	if (!bIsTimerSet)
	{
		BossCombatComponent->InitializeDamagingCircleTimer();
		bIsTimerSet = true;
	}
	
}
