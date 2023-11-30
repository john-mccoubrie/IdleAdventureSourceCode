


#include "IdleCharacterBase.h"
#include "AbilitySystem/IdleAbilitySystemComponent.h"

AIdleCharacterBase::AIdleCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create and attach the combat component
	CombatComponent = CreateDefaultSubobject<UCharacterCombatComponent>(TEXT("CharacterCombatComponent"));

}

UAbilitySystemComponent* AIdleCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


void AIdleCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AIdleCharacterBase::InitializePrimaryAttributes() const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(DefaultPrimaryAttributes);
	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(DefaultPrimaryAttributes, 1.f, ContextHandle);
	//GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AIdleCharacterBase::AddCharacterAbilities()
{
	UIdleAbilitySystemComponent* IdleASC = CastChecked<UIdleAbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority()) return;

	//IdleASC->AddCharacterAbilities(StartupAbilities);
}


