
#include "Actor/IdleInteractionComponent.h"
#include "Player/IdlePlayerController.h"
#include <Character/IdleCharacter.h>
#include <Kismet/KismetMathLibrary.h>
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include <Player/IdlePlayerState.h>
#include <Chat/GameChatManager.h>

// Sets default values for this component's properties
UIdleInteractionComponent::UIdleInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

    // Initialize the Audio Component
    StaffAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("StaffAudioComponent"));
    StaffAudioComponent->bAutoActivate = false;
    //StaffDinkAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("StaffDinkAudioComponent"));
    //StaffDinkAudioComponent->bAutoActivate = false;

}

void UIdleInteractionComponent::ClickTree(FHitResult TreeHit, APawn* PlayerPawn)
{
    CurrentTree = Cast<AIdleEffectActor>(TreeHit.GetActor());
    TargetTree = Cast<AIdleEffectActor>(TreeHit.GetActor());
}

void UIdleInteractionComponent::StartWoodcuttingAbility(APawn* PlayerPawn)
{
    UE_LOG(LogTemp, Warning, TEXT("StartWoodcuttingAbility IdleInteractionComponent"));
    // Step 1: Calculate direction to the tree from the character's current location
    FVector DirectionToTree = (TargetTree->GetActorLocation() - PlayerPawn->GetActorLocation()).GetSafeNormal();
    FRotator RotationTowardsTree = DirectionToTree.Rotation();

    // Preserve the current Pitch and Roll of the character
    RotationTowardsTree.Pitch = PlayerPawn->GetActorRotation().Pitch;
    RotationTowardsTree.Roll = PlayerPawn->GetActorRotation().Roll;

    // Step 2: Set the character's rotation to face that direction
    PlayerPawn->SetActorRotation(RotationTowardsTree);

    AIdleCharacter* MyCharacter = Cast<AIdleCharacter>(PlayerPawn);
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();


   
    PS->ActivateAbility(UWoodcuttingAbility::StaticClass());
    //PC->WoodcuttingEXPEffect();
    SpawnTreeCutEffect(PlayerPawn);
    PlayStaffCastingSound();
        

    AIdleEffectActor* MyIdleEffectActor = Cast<AIdleEffectActor>(TargetTree);
    if (MyIdleEffectActor)
    {
        // Unbind first to ensure no multiple bindings
        OnTreeClicked.RemoveDynamic(MyIdleEffectActor, &AIdleEffectActor::SetTreeLifespan);
        OnTreeClicked.AddDynamic(MyIdleEffectActor, &AIdleEffectActor::SetTreeLifespan);
        OnTreeClicked.Broadcast(MyIdleEffectActor);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Cast to AIdleEffectActor failed! player controller"));
    }

}

void UIdleInteractionComponent::SpawnTreeCutEffect(APawn* PlayerPawn)
{
    //UE_LOG(LogTemp, Warning, TEXT("SpawnTreeCutEffect IdleInteractionComponent"));
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdleCharacter* ParticleCharacter = Cast<AIdleCharacter>(PlayerPawn);
    if (!ParticleCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("No particle character"));
        return;
    }
    

    USkeletalMeshComponent* CharacterWeapon = ParticleCharacter->GetMesh();
    StaffEndLocation = CharacterWeapon->GetSocketLocation(FName("StaffEndSocket"));
    FVector TreeLocation = TargetTree->GetActorLocation();

    FRotator RotationTowardsTree = UKismetMathLibrary::FindLookAtRotation(StaffEndLocation, TreeLocation);
    RotationTowardsTree.Yaw += PC->YawRotationStaffMultiplier;
    RotationTowardsTree.Pitch += PC->PitchRotationStaffMultiplier;
    RotationTowardsTree.Roll += PC->RollRotationStaffMultiplier;
    //UE_LOG(LogTemp, Warning, TEXT("before Spawned effect"));
    // Spawn the Niagara effects
    if (GetWorld() && TreeCutEffect && TargetTree)
    {
        //UE_LOG(LogTemp, Warning, TEXT("Spawned effect"));
        SpawnedTreeEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TreeCutEffect, TargetTree->GetActorLocation());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Target Tree is null"));
    }
    
    SpawnedStaffEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), StaffEffect, StaffEndLocation, RotationTowardsTree);
}

void UIdleInteractionComponent::EndTreeCutEffect()
{
    if (SpawnedTreeEffect && SpawnedStaffEffect)
    {
        SpawnedTreeEffect->Deactivate();
        SpawnedStaffEffect->Deactivate();
    }
}

void UIdleInteractionComponent::PlayStaffCastingSound()
{
    /*
    if (StaffCastingSound && StaffAudioComponent && !StaffAudioComponent->IsPlaying())
    {
        StaffAudioComponent->SetSound(StaffCastingSound);
        StaffAudioComponent->Play();
    }
    */
    PlaySound(StaffCastingSound);
}

void UIdleInteractionComponent::StopStaffCastingSound()
{
    if (StaffAudioComponent && StaffAudioComponent->IsPlaying())
    {
        StaffAudioComponent->Stop();
    }
}

void UIdleInteractionComponent::PlayStaffDinkSound()
{
    /*
    if (StaffDinkSound && StaffDinkAudioComponent && !StaffDinkAudioComponent->IsPlaying())
    {
        StaffDinkAudioComponent->SetSound(StaffDinkSound);
        StaffDinkAudioComponent->Play();
        UE_LOG(LogTemp, Warning, TEXT("Staff dink sound reached."));
    }
    */

    PlaySound(StaffDinkSound);
}

void UIdleInteractionComponent::StopStaffDinkSound()
{
    if (StaffDinkAudioComponent && !StaffDinkAudioComponent->IsPlaying())
    {
        StaffDinkAudioComponent->Stop();
    }
}

void UIdleInteractionComponent::PlayCofferAddSound()
{
    PlaySound(AddToCofferSound);
}

void UIdleInteractionComponent::PlayAddToLegendaryMeterSound()
{
    PlaySound(AddToLegendaryMeterSound);
}

void UIdleInteractionComponent::PlayLegendaryTreeSpawnSound()
{
    PlaySound(LegendaryTreeSpawnSound);
}

void UIdleInteractionComponent::PlaySound(USoundBase* SoundToPlay)
{
    if (SoundToPlay && StaffAudioComponent && !StaffAudioComponent->IsPlaying())
    {
        StaffAudioComponent->SetSound(SoundToPlay);
        StaffAudioComponent->Play();
    }
}



void UIdleInteractionComponent::BeginPlay()
{
	Super::BeginPlay();


	
}


void UIdleInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

