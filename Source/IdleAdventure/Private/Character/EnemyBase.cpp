


#include "Character/EnemyBase.h"
#include <Combat/CombatManager.h>
#include "Character/IdleCharacter.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include <NiagaraFunctionLibrary.h>

// Sets default values
AEnemyBase::AEnemyBase()
{
	CombatComponent = CreateDefaultSubobject<UNPCCombatComponent>(TEXT("NPCCombatComponent"));

	//Set the default AI controller class
	NPCAIControllerClass = ANPCAIController::StaticClass();
}

void AEnemyBase::Interact()
{
	ACombatManager* CombatManager = ACombatManager::GetInstance(GetWorld());
	AIdleCharacter* PlayerCharacter = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	CombatManager->HandleCombat(PlayerCharacter->CombatComponent, this->CombatComponent);
}

void AEnemyBase::SpawnEnemyAttackEffect()
{
    // Play attack montage
    UAnimMontage* AnimMontage = EnemyAttackMontage;
    PlayAnimMontage(AnimMontage);

    //UE_LOG(LogTemp, Warning, TEXT("SpawnCombatEffect in Enemy_Goblin"));

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

void AEnemyBase::EndCombatEffects()
{
    if (SpawnedEnemyAttackEffect)
        SpawnedEnemyAttackEffect->Deactivate();
}

void AEnemyBase::EnemyAttacksPlayer()
{
    ACombatManager* CombatManager = ACombatManager::GetInstance(GetWorld());
    AIdleCharacter* PlayerCharacter = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    CombatManager->HandleCombat(this->CombatComponent, PlayerCharacter->CombatComponent);
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

