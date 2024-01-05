


#include "Character/Enemy_Ghoul.h"

AEnemy_Ghoul::AEnemy_Ghoul()
{
	CombatComponent = CreateDefaultSubobject<UNPCCombatComponent>(TEXT("NPCCombatComponent"));
}

void AEnemy_Ghoul::Interact()
{
	Super::Interact();
}

void AEnemy_Ghoul::SpawnEnemyAttackEffect()
{
	Super::SpawnEnemyAttackEffect();
}

void AEnemy_Ghoul::EndCombatEffects()
{
	Super::EndCombatEffects();
}

void AEnemy_Ghoul::EnemyAttacksPlayer()
{
	Super::EnemyAttacksPlayer();
}

void AEnemy_Ghoul::EnemyDeathAnimation()
{
	Super::EnemyDeathAnimation();
}

void AEnemy_Ghoul::UpdateWalkSpeed(float NewSpeed)
{
	Super::UpdateWalkSpeed(NewSpeed);
}
