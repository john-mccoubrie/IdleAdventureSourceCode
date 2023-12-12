
#include "Character/Enemy_Demon.h"
#include "GameFramework/CharacterMovementComponent.h" 

AEnemy_Demon::AEnemy_Demon()
{
	CombatComponent = CreateDefaultSubobject<UNPCCombatComponent>(TEXT("NPCCombatComponent"));
}

void AEnemy_Demon::Interact()
{
	Super::Interact();
}

void AEnemy_Demon::EquipWeapon()
{
	Super::EquipWeapon();
}

void AEnemy_Demon::SpawnEnemyAttackEffect()
{
	Super::SpawnEnemyAttackEffect();
}

void AEnemy_Demon::EndCombatEffects()
{
	Super::EndCombatEffects();
}

void AEnemy_Demon::EnemyAttacksPlayer()
{
	Super::EnemyAttacksPlayer();
}

void AEnemy_Demon::UpdateWalkSpeed(float NewSpeed)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NewSpeed + 500.0f;
	}
}
