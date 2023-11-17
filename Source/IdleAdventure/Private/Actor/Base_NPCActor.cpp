
#include "Actor/Base_NPCActor.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ABase_NPCActor::ABase_NPCActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a root component
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	// Create a capsule component and set it as the root component
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	//CapsuleComponent->InitCapsuleSize(730.f, 400.f); // Replace with your desired size
	CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));
	CapsuleComponent->ComponentTags.Add(FName("NPC"));
	CapsuleComponent->SetupAttachment(SceneRoot); // Attach the capsule to the scene root

	// Create and setup the mesh component to be attached to the capsule
	NPCMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NPCMeshComponent"));
	NPCMeshComponent->SetupAttachment(CapsuleComponent); // Attach the mesh to the capsule
}


void ABase_NPCActor::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("NPC_Base Interact"));
}

// Called when the game starts or when spawned
void ABase_NPCActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABase_NPCActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

