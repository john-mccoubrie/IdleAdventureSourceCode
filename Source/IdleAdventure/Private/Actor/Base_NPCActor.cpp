
#include "Actor/Base_NPCActor.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"

ABase_NPCActor::ABase_NPCActor()
{
	// Set this character to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize capsule size and collision properties
	//GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f); // Use your desired size
	//GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	GetCapsuleComponent()->ComponentTags.Add(FName("NPC"));

	// Attach and position the NPC mesh component
	NPCMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NPCMeshComponent"));
	NPCMeshComponent->SetupAttachment(GetCapsuleComponent());
	// Make sure this is set to position the mesh correctly relative to the capsule
	NPCMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));

	// Attach and position the indicator mesh component
	NPCIndicatorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NPCIndicatorMeshComponent"));
	NPCIndicatorMeshComponent->SetupAttachment(GetCapsuleComponent());
	// The DesiredHeightAboveNPC should be adjusted to position the indicator correctly
	NPCIndicatorMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + DesiredHeightAboveNPC));
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

