
#include "Actor/NPCActor.h"
#include "Components/CapsuleComponent.h"
#include <Player/IdlePlayerController.h>
#include <Quest/QuestManager.h>
#include <PlayFab/PlayFabManager.h>

// Sets default values
ANPCActor::ANPCActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;
	CapsuleComponent->ComponentTags.Add(FName("NPC"));
	CapsuleComponent->SetRelativeLocation(FVector(0.0f, 50.0f, CapsuleComponent->GetUnscaledCapsuleHalfHeight()));

	NPCMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NPCMeshComponent"));
	NPCMeshComponent->SetupAttachment(RootComponent);
}

void ANPCActor::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("NPC Interact"));
	AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
	PC->CurrentPlayerState = EPlayerState::Idle;

	//Open quest log
	OnInteractedWithNPC.Broadcast();

	//AQuestManager* QuestManager = AQuestManager::GetInstance(GetWorld());
	//QuestManager->GetQuestData();
}

void ANPCActor::AssignQuest(UQuest* Quest, AIdleCharacter* Player)
{
	if (Player && Quest)
	{
		// Ask the QuestManager to check if the quest can be accepted.
		AQuestManager* QuestManager = AQuestManager::GetInstance(GetWorld());
		if (QuestManager)
		{
			bool bCanAcceptQuest = !QuestManager->PlayerHasCompletedQuest(Quest);

			// Now, notify the player about the quest acceptance status.
			Player->NotifyQuestCompletionStatus(Quest->QuestID, bCanAcceptQuest);
			//Quest->OnQuestComplete.AddDynamic(this, &ANPCActor::CompleteQuest);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player or Quest is null"));
	}
}

void ANPCActor::CompleteQuest(UQuest* Quest, AIdleCharacter* Player)
{
	APlayFabManager* PlayFabManager = APlayFabManager::GetInstance(GetWorld());
	if (PlayFabManager && Player && Quest && Player->ActiveQuests.Contains(Quest))
	{
		PlayFabManager->CompleteQuest(Quest);
		Player->CompleteQuest(Quest);
	}
	
	if (Player && Quest && Player->ActiveQuests.Contains(Quest))
	{
		//Quest->Complete(); // Mark the quest as completed
		Quest->TurnInQuest();
		Player->CompleteQuest(Quest);
		//OnQuestCompleted.Broadcast(Quest); // Optional: Broadcast an event when a quest is completed.
	}
	
}

void ANPCActor::AddAvailableQuests(UQuest* QuestToAdd)
{
	AvailableQuests.Add(QuestToAdd);
}

bool ANPCActor::CanAcceptQuest(UQuest* Quest, AIdleCharacter* Player)
{
	APlayFabManager* PlayFabManager = APlayFabManager::GetInstance(GetWorld());
	if (PlayFabManager)
	{
		PlayFabManager->GetCompletedQuestVersion(Quest->QuestID, Player);
		// Note: Now you will handle the quest version in HandleQuestVersionRetrieved
	}

	return false;
}

void ANPCActor::HandleQuestVersionRetrieved(FString QuestID, FString QuestVersion)
{
	for (UQuest* AvailableQuest : AvailableQuests)
	{
		if (AvailableQuest->QuestID == QuestID)
		{
			if (QuestVersion != AvailableQuest->Version)
			{
				UE_LOG(LogTemp, Warning, TEXT("Quest %s is available for the player."), *QuestID);
				// Logic to make the quest available to the player
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Quest %s has already been completed by the player today."), *QuestID);
				// Logic to indicate the quest is not available
			}
			break;
		}
	}
}

// Called when the game starts or when spawned
void ANPCActor::BeginPlay()
{
	Super::BeginPlay();

	APlayFabManager* PlayFabManager = APlayFabManager::GetInstance(GetWorld());
	if (PlayFabManager)
	{
		PlayFabManager->OnQuestVersionRetrieved.AddDynamic(this, &ANPCActor::HandleQuestVersionRetrieved);
	}
	
}

// Called every frame
void ANPCActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

