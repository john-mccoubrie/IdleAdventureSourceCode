


#include "Combat/BaseCombatComponent.h"
#include <PlayerEquipment/BonusManager.h>
#include "GameFramework/Character.h"
#include "UI/DamageTextComponent.h"
#include "UI/StoicTypeIndicatorComponent.h"
#include <Game/SpawnManager.h>

// Sets default values for this component's properties
UBaseCombatComponent::UBaseCombatComponent()
{

	// Set default values
	MaxHealth = 100.0f;
	Health = MaxHealth;
	AttackRange = 200.0f; // Example value, adjust as needed
	Damage = 10.0f;
    BossIconOffset = 100.0f;
}

void UBaseCombatComponent::PerformAttack()
{
}

void UBaseCombatComponent::TakeDamage(float amount, float level)
{
	UE_LOG(LogTemp, Warning, TEXT("Base component Take Damage"));
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


// Called when the game starts
void UBaseCombatComponent::BeginPlay()
{
	Super::BeginPlay();

}


