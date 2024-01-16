


#include "Combat/BaseCombatComponent.h"
#include <PlayerEquipment/BonusManager.h>
#include "GameFramework/Character.h"
#include "UI/DamageTextComponent.h"
#include "UI/StoicTypeIndicatorComponent.h"
#include <Game/SpawnManager.h>
#include <Player/IdlePlayerController.h>
#include <Player/IdlePlayerState.h>
#include <AbilitySystem/IdleAttributeSet.h>

// Sets default values for this component's properties
UBaseCombatComponent::UBaseCombatComponent()
{

	// Set default values
	MaxHealth = 100.0f;
	Health = MaxHealth;
	AttackRange = 200.0f; // Example value, adjust as needed
	Damage = 10.0f;
    BossIconOffset = 100.0f;
    Experience = 100.0f;
    ScalingFactor = 0.4; //increaseing this value makes level difference between player and enemy matter less and vice versa

    EnemyAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EnemyAudioComponent"));
    EnemyAudioComponent->bAutoActivate = false;
}

void UBaseCombatComponent::PerformAttack()
{
}

void UBaseCombatComponent::TakeDamage(float amount, float level)
{
	UE_LOG(LogTemp, Warning, TEXT("Base component Take Damage"));
    /*
    ABonusManager* bonusManager = ABonusManager::GetInstance(GetWorld());

    // Retrieve and apply damage bonuses
    PendingDamage = amount + bonusManager->MultiplierSet.Damage;

    if (!GetWorld()->GetTimerManager().IsTimerActive(DamageCheckTimer))
    {
        // Start a repeating timer that checks for damage every second
        GetWorld()->GetTimerManager().SetTimer(DamageCheckTimer, this, &UBaseCombatComponent::DamageCheck, 1.0f, true);
    }
    */
}

void UBaseCombatComponent::IsAlive()
{
}

void UBaseCombatComponent::HandleDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Base component Handle Death"));

    //ASpawnManager* SpawnManager = ASpawnManager::GetInstance(GetWorld());
    //SpawnManager->UpdateEnemyCount(1);
}

void UBaseCombatComponent::AddHealth(float HealthToAdd)
{

}

void UBaseCombatComponent::DamageCheck()
{
    // Getting player's level from the player state
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);
    float playerLevel = IdleAttributeSet->GetWoodcuttingLevel(); // Replace with actual combat level attribute
    float enemyLevel = this->Level; // Enemy's level

    float baseDamage = PendingDamage; // Base damage

    // Define scaling factors
    float ScalingFactorHigher = 0.05; // Adjust as needed
    float ScalingFactorLower = -0.1; // Adjust as needed

    // Adjust hit probability based on player and enemy levels with distinction
    float levelDifference = enemyLevel - playerLevel; // Note the difference: enemyLevel - playerLevel
    float adjustedLevelDifference = (levelDifference > 0) ? levelDifference * ScalingFactorHigher : levelDifference * ScalingFactorLower;
    float hitProbability = FMath::Clamp(0.5f + adjustedLevelDifference, 0.1f, 0.9f); // Hit probability between 10% and 90%

    // Random roll to decide if the hit lands
    if (FMath::RandRange(0.0f, 1.0f) <= hitProbability)
    {
        // Adjusted damage range
        float minDamage = FMath::Max(1.0f, baseDamage * 0.1f); // Minimum damage is at least 1 or 10% of base damage
        float maxDamage = FMath::Max(baseDamage, minDamage); // Ensure max damage isn't lower than min

        // Generate single random damage
        float finalDamage = FMath::RandRange(minDamage, maxDamage);

        // Round to nearest whole number
        int32 finalDamageInt = FMath::RoundToInt(finalDamage);
        finalDamage = static_cast<float>(finalDamageInt);

        // Apply damage
        Health -= finalDamage;

        // Play hit sound
        PlayEnemeyHitSound();

        if (Health <= 0)
        {
            HandleDeath();
        }

        // Show damage number (red for damage, white for miss)
        ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
        if (OwningCharacter)
        {
            ShowDamageNumber(finalDamage, OwningCharacter, FLinearColor::Red);
        }
    }
    else
    {
        // Missed attack
        ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
        if (OwningCharacter)
        {
            ShowDamageNumber(0, OwningCharacter, FLinearColor::White);
        }
    }

    // Broadcast health change
    OnHealthChanged.Broadcast(Health, MaxHealth);
}

void UBaseCombatComponent::ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, FSlateColor Color)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass)
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, Color);
	}
}

void UBaseCombatComponent::ChangeStoicImage(FString StoicType, ACharacter* TargetCharacter)
{
    if (IsValid(TargetCharacter))
    {
        // Attempt to find the existing StoicTypeIcon component
        UStoicTypeIndicatorComponent* StoicText = Cast<UStoicTypeIndicatorComponent>(
            TargetCharacter->GetComponentByClass(UStoicTypeIndicatorComponent::StaticClass())
        );

        if (StoicText)
        {
            // If the component exists, just update the stoic type
            StoicText->SetStoicType(StoicType);

            // Adjust the Z-value to move the widget up
            //FVector NewLocation = StoicText->GetRelativeLocation();
            //NewLocation.Z += BossIconOffset; // Set your desired offset value here
            //StoicText->SetRelativeLocation(NewLocation);
        }
        else
        {
            // If it doesn't exist, create a new component and attach it
            StoicText = NewObject<UStoicTypeIndicatorComponent>(TargetCharacter, StoicTypeComponentClass);
            if (StoicText)
            {
                StoicText->RegisterComponent();
                StoicText->AttachToComponent(TargetCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);

                // Adjust the Z-value to move the widget up after attachment
                FVector NewLocation = StoicText->GetRelativeLocation();
                NewLocation.Z += BossIconOffset; // Set your desired offset value here
                StoicText->SetRelativeLocation(NewLocation);

                StoicText->SetStoicType(StoicType);
            }
        }
    }
}


void UBaseCombatComponent::PlaySound(USoundBase* SoundToPlay)
{
    if (SoundToPlay && EnemyAudioComponent)
    {
        // Stop the currently playing sound if any
        if (EnemyAudioComponent->IsPlaying())
        {
            EnemyAudioComponent->Stop();
        }

        // Set the new sound and play it
        EnemyAudioComponent->SetSound(SoundToPlay);
        EnemyAudioComponent->Play();
    }
}

void UBaseCombatComponent::PlayEnemeyHitSound()
{
    PlaySound(EnemyHitSound);
}

void UBaseCombatComponent::StopCircleDamageCheckTimer()
{
    UE_LOG(LogTemp, Warning, TEXT("StopCircleDamageCheckTimer called in base combat component"));
}

void UBaseCombatComponent::StartCircleDamageCheckTimer()
{
    UE_LOG(LogTemp, Warning, TEXT("StartCircleDamageCheckTimer called in base combat component"));
}

// Called when the game starts
void UBaseCombatComponent::BeginPlay()
{
	Super::BeginPlay();

}


