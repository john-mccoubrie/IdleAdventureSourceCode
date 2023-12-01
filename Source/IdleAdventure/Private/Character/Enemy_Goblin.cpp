

#include "Character/Enemy_Goblin.h"
#include <Combat/CombatManager.h>
#include "Combat/CharacterCombatComponent.h"
#include "Character/IdleCharacter.h"
#include <Kismet/GameplayStatics.h>
#include <Player/IdlePlayerController.h>
#include <Kismet/KismetMathLibrary.h>
#include <NiagaraFunctionLibrary.h>

void AEnemy_Goblin::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy goblin interact called)"));
	//ACombatManager* CombatManager = ACombatManager::GetInstance(GetWorld());
	//AIdleCharacter* PlayerCharacter = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    //Player attacks the character
	//CombatManager->HandleCombat(PlayerCharacter->CombatComponent, this->CombatComponent);
	//SpawnEnemyAttackEffect();
    //EnemyAttacksPlayer();
}

void AEnemy_Goblin::SpawnEnemyAttackEffect()
{
    
}

void AEnemy_Goblin::EndCombatEffects()
{
    
}

void AEnemy_Goblin::EnemyAttacksPlayer()
{
    
}
