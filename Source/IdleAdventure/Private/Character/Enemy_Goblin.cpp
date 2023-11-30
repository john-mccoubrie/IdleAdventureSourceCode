

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
	ACombatManager* CombatManager = ACombatManager::GetInstance(GetWorld());
	AIdleCharacter* PlayerCharacter = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    //Player attacks the character
	CombatManager->HandleCombat(PlayerCharacter->CombatComponent, this->CombatComponent);
	//SpawnEnemyAttackEffect();
    //EnemyAttacksPlayer();
}

void AEnemy_Goblin::SpawnEnemyAttackEffect()
{
    // Play attack montage
    UAnimMontage* AnimMontage = EnemyAttackMontage;
    PlayAnimMontage(AnimMontage);

    UE_LOG(LogTemp, Warning, TEXT("SpawnCombatEffect in Enemy_Goblin"));

    // Get player character
    AIdleCharacter* PlayerCharacter = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (!PlayerCharacter)
    {
        return; // Exit if player character is not found
    }

    // Get enemy location
    FVector EnemyLocation = GetActorLocation();
    FVector PlayerLocation = PlayerCharacter->GetActorLocation();

    // Calculate rotation towards player
    FRotator RotationTowardsPlayer = UKismetMathLibrary::FindLookAtRotation(EnemyLocation, PlayerLocation);

    // Set enemy rotation to face the player
    SetActorRotation(RotationTowardsPlayer);

    // Spawn effect on the Player
    SpawnedEnemyAttackEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), EnemyAttackEffect, PlayerLocation);
}

void AEnemy_Goblin::EndCombatEffects()
{
    if(SpawnedEnemyAttackEffect)
    SpawnedEnemyAttackEffect->Deactivate();
}

void AEnemy_Goblin::EnemyAttacksPlayer()
{
    ACombatManager* CombatManager = ACombatManager::GetInstance(GetWorld());
    AIdleCharacter* PlayerCharacter = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    CombatManager->HandleCombat(this->CombatComponent, PlayerCharacter->CombatComponent);
}
