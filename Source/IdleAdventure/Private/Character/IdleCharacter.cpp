#include "Character/IdleCharacter.h"
#include "AbilitySystemComponent.h"
#include "Player/IdlePlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/IdlePlayerState.h"
#include "UI/HUD/IdleHUD.h"
#include "Animation/AnimMontage.h"
#include <AbilitySystem/IdleAttributeSet.h>
#include <PlayerEquipment/EquipmentManager.h>
#include <PlayerEquipment/PlayerEquipment.h>

AIdleCharacter::AIdleCharacter()
{
	UE_LOG(LogTemp, Warning, TEXT("Character constructor called"));
	//Inventory = CreateDefaultSubobject<AInventory>(TEXT("Inventory"));


	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	
	//AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &AIdleCharacter::OnActiveGameplayEffectAddedCallback);

	
}

void AIdleCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//Init ability actor infor for the server
	InitAbilityActorInfo();
	AddCharacterAbilities();
}

void AIdleCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//Init ability actor info for the client
	InitAbilityActorInfo();
}

void AIdleCharacter::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CharacterInventory = GetWorld()->SpawnActor<AInventory>(AInventory::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
}


void AIdleCharacter::InitAbilityActorInfo()
{
	AIdlePlayerState* IdlePlayerState = GetPlayerState<AIdlePlayerState>();
	check(IdlePlayerState);
	IdlePlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(IdlePlayerState, this);
	AbilitySystemComponent = IdlePlayerState->GetAbilitySystemComponent();
	AttributeSet = IdlePlayerState->GetAttributeSet();

	if (AIdlePlayerController* IdlePlayerController = Cast<AIdlePlayerController>(GetController()))
	{
		if (AIdleHUD* IdleHUD = Cast<AIdleHUD>(IdlePlayerController->GetHUD()))
		{
			IdleHUD->InitOverlay(IdlePlayerController, IdlePlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	InitializePrimaryAttributes();
}

