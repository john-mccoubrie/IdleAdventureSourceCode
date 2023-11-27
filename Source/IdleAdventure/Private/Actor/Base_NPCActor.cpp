
#include "Actor/Base_NPCActor.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABase_NPCActor::ABase_NPCActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	/*
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;
	CapsuleComponent->SetRelativeLocation(FVector(0.0f, 50.0f, CapsuleComponent->GetUnscaledCapsuleHalfHeight()));
	NPCMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NPCMeshComponent"));
	NPCMeshComponent->SetupAttachment(RootComponent);
	*/
	
	// Create a root component
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	// Create a capsule component and set it as the root component
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->InitCapsuleSize(730.f, 400.f); // Replace with your desired size
	//CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));
	CapsuleComponent->ComponentTags.Add(FName("NPC"));
	CapsuleComponent->SetupAttachment(SceneRoot); // Attach the capsule to the scene root

	// Create and setup the mesh component to be attached to the capsule
	NPCMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NPCMeshComponent"));
	NPCMeshComponent->SetupAttachment(CapsuleComponent); // Attach the mesh to the capsule
	
	// Create the money sign mesh component
	NPCIndicatorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MoneySignMeshComponent"));
	NPCIndicatorMeshComponent->SetupAttachment(CapsuleComponent); // or SceneRoot if you prefer

	// Position it above the NPC
	NPCIndicatorMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, DesiredHeightAboveNPC)); // Set DesiredHeightAboveNPC to your desired value

	
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

	/*
	// Draw the collision capsule only if the game is in the correct state (e.g., a debug mode is active)
	if (GetWorld()) // bIsDebugModeActive is a hypothetical variable you would use to toggle debug drawing
	{
		DrawDebugCapsule(
			GetWorld(),
			CapsuleComponent->GetComponentLocation(),
			CapsuleComponent->GetScaledCapsuleHalfHeight(),
			CapsuleComponent->GetScaledCapsuleRadius(),
			CapsuleComponent->GetComponentQuat(),
			FColor::Green, // Collision shape color
			false, // Persistent lines
			-1.0f, // Duration of the draw
			0, // Depth priority
			1.0f // Thickness of the lines
		);
	}
	*/
}

