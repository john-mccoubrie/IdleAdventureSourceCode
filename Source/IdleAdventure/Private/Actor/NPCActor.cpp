
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
		if (CanAcceptQuest(Quest))
		{
			Player->AddQuest(Quest);
			AvailableQuests.Remove(Quest);
			//OnQuestAssigned.Broadcast(Quest);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Player has already completed their daily quest."));
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
		PlayFabManager->CompleteQuest(Quest, Player);
		Player->CompleteQuest(Quest);
	}
	/*
	if (Player && Quest && Player->ActiveQuests.Contains(Quest))
	{
		//Quest->Complete(); // Mark the quest as completed
		Quest->TurnInQuest();
		Player->CompleteQuest(Quest);
		//OnQuestCompleted.Broadcast(Quest); // Optional: Broadcast an event when a quest is completed.
	}
	*/
}

void ANPCActor::AddAvailableQuests(UQuest* QuestToAdd)
{
	AvailableQuests.Add(QuestToAdd);
}

bool ANPCActor::CanAcceptQuest(UQuest* Quest)
{
	APlayFabManager* PlayFabManager = APlayFabManager::GetInstance(GetWorld());
	if (PlayFabManager)
	{
		PlayFabManager->GetCompletedQuestVersion(Quest->QuestID);
		// Note: Now you will handle the quest version in HandleQuestVersionRetrieved
	}

	return false;
}

void ANPCActor::HandleQuestVersionRetrieved(FString QuestID, FString QuestVersion)
{
	// Assuming AvailableQuests is a TArray or TMap containing your quests
	for (UQuest* AvailableQuest : AvailableQuests)
	{
		if (AvailableQuest->QuestID == QuestID) // Assuming QuestID is a member of your UQuest class
		{
			if (QuestVersion != AvailableQuest->Version)
			{
				UE_LOG(LogTemp, Warning, TEXT("Quest %s is available for the player."), *QuestID);

				// Additional logic here such as adding the quest to the player’s available quests UI
				// You might also enable interaction points or NPCs associated with this quest
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Quest %s has already been completed by the player today."), *QuestID);

				// Additional logic such as disabling the quest in the player’s available quests UI
				// You might also disable interaction points or NPCs associated with this quest
			}

			break; // Exit the loop once the quest is found
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

