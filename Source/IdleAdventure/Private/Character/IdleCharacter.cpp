#include "Character/IdleCharacter.h"
#include "AbilitySystemComponent.h"
#include "Quest/QuestEnums.h"
#include "Player/IdlePlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/IdlePlayerState.h"
#include "UI/HUD/IdleHUD.h"
#include "Animation/AnimMontage.h"
#include <AbilitySystem/IdleAttributeSet.h>
#include <PlayerEquipment/EquipmentManager.h>
#include <PlayerEquipment/PlayerEquipment.h>
#include <Quest/QuestManager.h>

AIdleCharacter::AIdleCharacter()
{
	//UE_LOG(LogTemp, Warning, TEXT("Character constructor called"));
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

	SpringArm = FindComponentByClass<USpringArmComponent>();
	Camera = FindComponentByClass<UCameraComponent>();
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

void AIdleCharacter::AddQuest(UQuest* Quest)
{
	if (Quest && !ActiveQuests.Contains(Quest))
	{
		ActiveQuests.Add(Quest);
		Quest->Start(); // Assuming Start initializes quest status
		

		// Notify the UI to update the quest log display
		UpdateQuestLogUI();
	}
}

void AIdleCharacter::CompleteQuest(UQuest* Quest)
{
	// Assuming Complete finalizes the quest status
		// This should now only be called when turning in the quest
		// Quest->Complete(); // This line should be removed or commented out
// 
	//create an awards function
		// Update the character state, e.g., give rewards
	
		// Remove quest from active quests if it's completed
	if (Quest->QuestState == EQuestState::Completed)
	{
		ActiveQuests.Remove(Quest);
	}

	// Notify the UI to update the quest log display
	UpdateQuestLogUI();

}

void AIdleCharacter::UpdateQuestLogUI()
{
	
	//OnActiveQuestsUpdated.Broadcast();
}

UQuest* AIdleCharacter::GetCurrentActiveQuest()
{
	// This is a simple example where we return the last quest added to the ActiveQuests array.
	// You might have more complex logic to determine the current active quest.
	if (ActiveQuests.Num() > 0)
	{
		return ActiveQuests.Last();
	}

	return nullptr; // Return nullptr if there are no active quests.
}

void AIdleCharacter::UpdateAllActiveQuests(const FString& ObjectiveType, int32 Amount)
{
	for (UQuest* Quest : ActiveQuests)
	{
		Quest->UpdateProgress(ObjectiveType, Amount);
	}
}

