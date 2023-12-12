


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

void AEnemy_Demon_Boss::EquipWeapon()
{
	Super::EquipWeapon();
}

void AEnemy_Demon_Boss::SpawnEnemyAttackEffect()
{
	Super::SpawnEnemyAttackEffect();
}

void AEnemy_Demon_Boss::EndCombatEffects()
{
	Super::EndCombatEffects();
}

void AEnemy_Demon_Boss::EnemyAttacksPlayer()
{
	Super::EnemyAttacksPlayer();
	
	UE_LOG(LogTemp, Warning, TEXT("demon boss attacks player"));
	ACombatManager* CombatManager = ACombatManager::GetInstance(GetWorld());
	AIdleCharacter* PlayerCharacter = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	CombatManager->HandleCombat(DemonBossCombatComponent, PlayerCharacter->CombatComponent, 0);
	
}
