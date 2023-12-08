

#include "Character/Enemy_Goblin.h"
#include <Combat/CombatManager.h>
#include "Combat/CharacterCombatComponent.h"
#include "Character/IdleCharacter.h"
#include <Kismet/GameplayStatics.h>
#include <Player/IdlePlayerController.h>
#include <Kismet/KismetMathLibrary.h>
#include <NiagaraFunctionLibrary.h>

AEnemy_Goblin::AEnemy_Goblin()
{
	CombatComponent = CreateDefaultSubobject<UNPCCombatComponent>(TEXT("NPCCombatComponent"));
}

void AEnemy_Goblin::Interact()
{
	Super::Interact();

	UE_LOG(LogTemp, Warning, TEXT("Enemy goblin interact called)"));
	//ACombatManager* CombatManager = ACombatManager::GetInstance(GetWorld());
	//AIdleCharacter* PlayerCharacter = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    //Player attacks the character
	//CombatManager->HandleCombat(PlayerCharacter->CombatComponent, this->CombatComponent);
	//SpawnEnemyAttackEffect();
    //EnemyAttacksPlayer();
}

void AEnemy_Goblin::EquipWeapon()
{
	Super::EquipWeapon();
}

void AEnemy_Goblin::SpawnEnemyAttackEffect()
{
	Super::SpawnEnemyAttackEffect();
}

void AEnemy_Goblin::EndCombatEffects()
{
	Super::EndCombatEffects();
}

void AEnemy_Goblin::EnemyAttacksPlayer()
{
	Super::EnemyAttacksPlayer();
}
