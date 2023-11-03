
#include "Actor/NPCActor.h"
#include "Components/CapsuleComponent.h"
#include <Player/IdlePlayerController.h>
#include <Quest/QuestManager.h>

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
		Player->AddQuest(Quest);
		AvailableQuests.Remove(Quest);
		//OnQuestAssigned.Broadcast(Quest);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player or Quest is null"));
	}
}

void ANPCActor::CompleteQuest(UQuest* Quest, AIdleCharacter* Player)
{
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

// Called when the game starts or when spawned
void ANPCActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANPCActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

