


#include "Combat/BaseCombatComponent.h"
#include <PlayerEquipment/BonusManager.h>
#include "GameFramework/Character.h"
#include "UI/DamageTextComponent.h"

// Sets default values for this component's properties
UBaseCombatComponent::UBaseCombatComponent()
{

	// Set default values
	MaxHealth = 100.0f;
	Health = MaxHealth;
	AttackRange = 200.0f; // Example value, adjust as needed
	Damage = 10.0f;
}

void UBaseCombatComponent::PerformAttack()
{
}

void UBaseCombatComponent::TakeDamage(float amount)
{
	UE_LOG(LogTemp, Warning, TEXT("Base component Take Damage"));
}

void UBaseCombatComponent::IsAlive()
{
}

void UBaseCombatComponent::HandleDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Base component Handle Death"));
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


// Called when the game starts
void UBaseCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


