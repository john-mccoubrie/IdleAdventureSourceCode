


#include "Character/EnemyBase.h"
#include <Combat/CombatManager.h>
#include "Combat/CharacterCombatComponent.h"
#include "Character/IdleCharacter.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CombatComponent = CreateDefaultSubobject<UNPCCombatComponent>(TEXT("NPCCombatComponent"));

	//Set the default AI controller class
	NPCAIControllerClass = ANPCAIController::StaticClass();
}

void AEnemyBase::Interact()
{

}

void AEnemyBase::EndCombatEffects()
{

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

