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
#include <PlayFab/PlayFabManager.h>

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

	// Initialize PlayFabManager reference
	
	AQuestManager* QuestManager = AQuestManager::GetInstance(GetWorld());


	// Check the completion status of all quests on game start
	for (UQuest* Quest : QuestManager->AvailableQuests) // Assuming AvailableQuests is an array of all quests
	{
		CheckQuestCompletionFromPlayFab(Quest);
	}
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
		UE_LOG(LogTemp, Warning, TEXT("added an active quest"));
		ActiveQuests.Add(Quest);
		Quest->Start(); // Assuming Start initializes quest status
		// Notify the UI to update the quest log display
		UpdateQuestLogUI();
	}
}

void AIdleCharacter::CompleteQuest(UQuest* Quest)
{
	if (Quest && Quest->QuestState == EQuestState::Completed)
	{
		APlayFabManager* PlayFabManager = APlayFabManager::GetInstance(GetWorld());
		PlayFabManager->CompleteQuest(Quest);
		UE_LOG(LogTemp, Warning, TEXT("Complete quest called in character"));

		// Remove quest from active quests
		ActiveQuests.Remove(Quest);

		// Notify the UI to update the quest log display
		UpdateQuestLogUI();
	}

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

void AIdleCharacter::NotifyQuestCompletionStatus(FString QuestID, bool bCanAccept)
{
	if (bCanAccept)
	{
		// Logic to accept the quest
		// Find the quest by ID and add it to the active quests if it's not already there
		AQuestManager* QuestManager = AQuestManager::GetInstance(GetWorld());
		UQuest** QuestPointer = QuestManager->AvailableQuests.FindByPredicate([&](UQuest* Quest) {
			return Quest && Quest->QuestID.Equals(QuestID);
			});

		UQuest* QuestToAccept = nullptr;
		if (QuestPointer != nullptr)
		{
			QuestToAccept = *QuestPointer;
		}

		if (QuestToAccept)
		{
			AddQuest(QuestToAccept);
			UE_LOG(LogTemp, Warning, TEXT("accept quest"));
		}
	}
	else
	{
		// Logic to reject the quest because it's already completed
		UE_LOG(LogTemp, Warning, TEXT("Player has already completed their daily quest."));
	}
}

bool AIdleCharacter::HasQuestWithVersion(const FString& QuestID, const FString& Version) const
{
	// Iterate through the ActiveQuests array
	for (UQuest* Quest : ActiveQuests)
	{
		// Check if the current quest matches the QuestID
		if (Quest && Quest->QuestID.Equals(QuestID))
		{
			// Check if the version of the current quest matches the provided Version
			if (Quest->Version.Equals(Version))
			{
				// The quest with this version is active
				return true;
			}
		}
	}

	// If no matching quest is found, return false
	return false;
}

void AIdleCharacter::CheckQuestCompletionFromPlayFab(UQuest* Quest)
{
	APlayFabManager* PlayFabManager = APlayFabManager::GetInstance(GetWorld());
	if (PlayFabManager && Quest)
	{
		PlayFabManager->CheckIfQuestCompleted(Quest, this);
	}
}
