
#include "Actor/IdleInteractionComponent.h"
#include "Player/IdlePlayerController.h"
#include <Character/IdleCharacter.h>
#include "Actor/IdleActorManager.h"
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

    EquipAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EquipAudioComponent"));
    EquipAudioComponent->bAutoActivate = false;

    WorldAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("WorldAudioComponent"));
    WorldAudioComponent->bAutoActivate = false;
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

void UIdleInteractionComponent::StartCombatAbility(APawn* PlayerPawn, AActor* Enemy)
{
    // Rotate PlayerPawn to face Enemy
    UE_LOG(LogTemp, Warning, TEXT("Start Combat Ability in IdleInteractionComponent"));
    //CurrentPlayerState = EPlayerState::InCombat;
    // Step 1: Calculate direction to the enemy from the character's current location
    FVector DirectionToEnemy = (Enemy->GetActorLocation() - PlayerPawn->GetActorLocation()).GetSafeNormal();
    FRotator RotationTowardsEnemy = DirectionToEnemy.Rotation();

    // Preserve the current Pitch and Roll of the character
    RotationTowardsEnemy.Pitch = PlayerPawn->GetActorRotation().Pitch;
    RotationTowardsEnemy.Roll = PlayerPawn->GetActorRotation().Roll;

    // Step 2: Set the character's rotation to face that direction
    PlayerPawn->SetActorRotation(RotationTowardsEnemy);

    // Start combat animations or abilities
    //SpawnCombatEffect(PlayerPawn, Enemy);

    // If using Gameplay Abilities:
    // PS->ActivateAbility(UCombatAbility::StaticClass());

    // If using Animation Montages:
    AIdleCharacter* Character = Cast<AIdleCharacter>(PlayerPawn);
    UAnimMontage* AnimMontage = Character->PlayerAttackMontage;
    Character->PlayAnimMontage(AnimMontage);

    // You can also spawn particle effects or play sounds here
    // ...

    // Handle damage dealing
    // ...
}

void UIdleInteractionComponent::SpawnTreeCutEffect(APawn* PlayerPawn)
{
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdleCharacter* ParticleCharacter = Cast<AIdleCharacter>(PlayerPawn);
    if (!ParticleCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("No particle character"));
        return;
    }

    USkeletalMeshComponent* CharacterWeapon = ParticleCharacter->GetMesh();
    StaffEndLocation = CharacterWeapon->GetSocketLocation(FName("StaffEndSocket"));

    // Existing logic to set the rotation towards the tree
    FVector TreeLocation = TargetTree->GetActorLocation();

    // New raycasting logic to find the ground location beneath the tree
    FHitResult Hit;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(TargetTree);
    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TreeLocation, FVector(TreeLocation.X, TreeLocation.Y, -10000.0f), ECC_WorldStatic, CollisionParams);

    FVector EffectLocation;
    if (bHit)
    {
        // Position the tree effect a certain distance above the ground
        float DistanceAboveGround = 50.0f; // Adjust as needed
        EffectLocation = Hit.ImpactPoint + FVector(0.0f, 0.0f, DistanceAboveGround);
    }
    else
    {
        // Default to tree base location if raycast didn't hit
        FVector TreeBoundsMin, TreeBoundsMax;
        TargetTree->GetActorBounds(false, TreeBoundsMin, TreeBoundsMax);
        EffectLocation = FVector(TreeLocation.X, TreeLocation.Y, TreeBoundsMin.Z);
    }

    // Calculate the horizontal direction towards the tree cut effect location
    FVector HorizontalDirection = (EffectLocation - StaffEndLocation);
    HorizontalDirection.Z = 0; // Ignore the vertical component
    FRotator HorizontalRotation = HorizontalDirection.Rotation();

    // Apply additional rotation adjustments if necessary
    HorizontalRotation.Yaw += PC->YawRotationStaffMultiplier;
    // Optionally, remove or modify the Pitch and Roll adjustments as they might not be needed
    // HorizontalRotation.Pitch += PC->PitchRotationStaffMultiplier;
    // HorizontalRotation.Roll += PC->RollRotationStaffMultiplier;

    // Spawn the tree effect at the calculated location
    if (GetWorld() && TreeCutEffect && TargetTree)
    {
        SpawnedTreeEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TreeCutEffect, EffectLocation);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Target Tree is null"));
    }

    // Spawn the staff effect with the new horizontal rotation
    SpawnedStaffEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), StaffEffect, StaffEndLocation, HorizontalRotation);
}

void UIdleInteractionComponent::EndTreeCutEffect()
{
    if (SpawnedTreeEffect && SpawnedStaffEffect)
    {
        if (IsValid(SpawnedTreeEffect))
        {
            SpawnedTreeEffect->Deactivate();
            SpawnedTreeEffect = nullptr;
        }
        if (IsValid(SpawnedStaffEffect))
        {
            SpawnedStaffEffect->Deactivate();
            SpawnedStaffEffect = nullptr;
        }
    }
}

void UIdleInteractionComponent::SpawnCombatEffect(APawn* PlayerPawn, AEnemyBase* TargetEnemy)
{
    UE_LOG(LogTemp, Warning, TEXT("SpawnCombatEffect in IdleInteractionComponent"));
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdleCharacter* ParticleCharacter = Cast<AIdleCharacter>(PlayerPawn);
    if (!ParticleCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("No particle character"));
        return;
    }


    USkeletalMeshComponent* CharacterWeapon = ParticleCharacter->GetMesh();
    StaffEndLocation = CharacterWeapon->GetSocketLocation(FName("StaffEndSocket"));
    FVector EnemyLocation = TargetEnemy->GetActorLocation();

    FRotator RotationTowardsEnemy = UKismetMathLibrary::FindLookAtRotation(StaffEndLocation, EnemyLocation);
    RotationTowardsEnemy.Yaw += PC->YawRotationStaffMultiplier;
    RotationTowardsEnemy.Pitch += PC->PitchRotationStaffMultiplier;
    RotationTowardsEnemy.Roll += PC->RollRotationStaffMultiplier;
    //UE_LOG(LogTemp, Warning, TEXT("before Spawned effect"));
    // Spawn the Niagara effects
    if (GetWorld() && TargetEnemy)
    {
        // Spawn the effect was EndOfStaffLocation
        SpawnedAttackEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PlayerAttackEffect, EnemyLocation, RotationTowardsEnemy);

        // Set a timer to automatically destroy the effect after 2 seconds
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
            if (SpawnedAttackEffect)
            {
                EndCombatEffect();
            }
            }, 2.0f, false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Target Tree is null"));
    }

    //SpawnedAttackEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), StaffAttackEffect, StaffEndLocation, RotationTowardsEnemy);
  
}

void UIdleInteractionComponent::EndCombatEffect()
{
    if (SpawnedAttackEffect)
    {
        SpawnedAttackEffect->Deactivate();
        SpawnedAttackEffect->DestroyComponent();
        SpawnedAttackEffect = nullptr; // Setting it to nullptr to avoid dangling references
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

void UIdleInteractionComponent::PlayInventoryFullSound()
{
    PlaySound(InventoryFullSound);
}

void UIdleInteractionComponent::PlayAddToLegendaryMeterSound()
{
    PlaySound(AddToLegendaryMeterSound);
}

void UIdleInteractionComponent::PlayLegendaryTreeSpawnSound()
{
    PlayWorldSound(LegendaryTreeSpawnSound);
}

void UIdleInteractionComponent::SpawnLevelUpEffect(APawn* PlayerPawn)
{
    if (!PlayerPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerPawn is null"));
        return;
    }

    // Get the root component or a specific component like a skeletal mesh to attach the effect
    USceneComponent* AttachComponent = PlayerPawn->GetRootComponent();

    if (GetWorld() && LevelUpEffect)
    {
        // If there is an existing effect, stop it before spawning a new one
        if (SpawnedLevelUpEffect)
        {
            SpawnedLevelUpEffect->Deactivate();
            SpawnedLevelUpEffect->DestroyComponent();
        }

        // Spawn the effect and attach it to the component
        SpawnedLevelUpEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(LevelUpEffect, AttachComponent, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);

        // Set a timer to stop the effect after 3 seconds
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UIdleInteractionComponent::StopLevelUpEffect, 3.0f, false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("LevelUpEffect is null or world is not valid"));
    }
}

void UIdleInteractionComponent::StopLevelUpEffect()
{
    if (SpawnedLevelUpEffect)
    {
        SpawnedLevelUpEffect->Deactivate();
        SpawnedLevelUpEffect->DestroyComponent();
        SpawnedLevelUpEffect = nullptr;
    }
}

void UIdleInteractionComponent::PlayLevelUpSound()
{
    PlaySound(LevelUpSound);
}

void UIdleInteractionComponent::PlayEquipSound()
{
    if (EquipSound && EquipAudioComponent && !EquipAudioComponent->IsPlaying())
    {
        EquipAudioComponent->SetSound(EquipSound);
        EquipAudioComponent->Play();
    }
}

void UIdleInteractionComponent::PlayQuestReadyForTurnInSound()
{
    PlaySound(QuestReadyForTurnInSound);
}

void UIdleInteractionComponent::PlayQuestTurnInSound()
{
    PlaySound(QuestTurnInSound);
}

void UIdleInteractionComponent::PlayRunCompleteSound()
{
    PlayWorldSound(RunCompleteSound);
}

void UIdleInteractionComponent::PlayPickupPotionSound()
{
    PlayWorldSound(PickupPotionSound);
}

void UIdleInteractionComponent::PlayDialogueClickSound()
{
    PlayWorldSound(DialogueClickSound);
}

void UIdleInteractionComponent::PlaySound(USoundBase* SoundToPlay)
{
    if (SoundToPlay && StaffAudioComponent)
    {
        // Stop the currently playing sound if any
        if (StaffAudioComponent->IsPlaying())
        {
            StaffAudioComponent->Stop();
        }

        // Set the new sound and play it
        StaffAudioComponent->SetSound(SoundToPlay);
        StaffAudioComponent->Play();
    }
}

void UIdleInteractionComponent::PlayWorldSound(USoundBase* SoundToPlay)
{
    if (SoundToPlay && WorldAudioComponent)
    {
        // Stop the currently playing sound if any
        if (WorldAudioComponent->IsPlaying())
        {
            WorldAudioComponent->Stop();
        }

        // Set the new sound and play it
        WorldAudioComponent->SetSound(SoundToPlay);
        WorldAudioComponent->Play();
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

