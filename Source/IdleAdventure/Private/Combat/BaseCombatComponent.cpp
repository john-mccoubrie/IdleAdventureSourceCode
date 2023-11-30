


#include "Combat/BaseCombatComponent.h"

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
	Health -= amount;
	UE_LOG(LogTemp, Warning, TEXT("Defender took %f damage!"), amount);
	UE_LOG(LogTemp, Warning, TEXT("They now have %f health!"), Health);
	if (Health <= 0)
	{
		//Handle death
		HandleDeath();
	}

	OnHealthChanged.Broadcast(this, Health, MaxHealth);
}

void UBaseCombatComponent::IsAlive()
{
}

void UBaseCombatComponent::HandleDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Handle Death"));
}


// Called when the game starts
void UBaseCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


