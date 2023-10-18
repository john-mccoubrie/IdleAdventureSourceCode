#include "Actor/IdleEffectActor.h"
#include "EngineUtils.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include <Player/IdlePlayerState.h>
#include "Components/SphereComponent.h"
#include <Player/IdlePlayerController.h>
#include <AbilitySystem/Abilities/WoodcuttingAbility.h>
#include <Character/IdleCharacter.h>
#include <Actor/IdleActorManager.h>

AIdleEffectActor::AIdleEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;

	TreeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TreeMeshComponent"));
	TreeMeshComponent->SetupAttachment(RootComponent);

	// Initialize and attach the Niagara system component
	LegendaryEffectParticle = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LegendaryEffectParticle"));
	LegendaryEffectParticle->SetupAttachment(RootComponent);
	LegendaryEffectParticle->SetAutoActivate(false); // Don't auto-activate the particle effect

}

void AIdleEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AIdleEffectActor::SetTreeLifespan(AIdleEffectActor* Tree)
{
	//UE_LOG(LogTemp, Warning, TEXT("Set Tree Lifespan"));
	
	//Get the ability system on the tree
	AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
	AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (Tree != nullptr) {
		if (ASC != nullptr) 
		{
			//Get instance to the woodcutting ability
			//UWoodcuttingAbility* WoodcuttingAbility = PC->CurrentWoodcuttingAbilityInstance;
			if (PC->CurrentWoodcuttingAbilityInstance)
			{
				//Get instance of the woodcutting ability active handle
				FActiveGameplayEffectHandle WoodcuttingActiveEffectHandle = PC->CurrentWoodcuttingAbilityInstance->GetActiveEffectHandle();

				//Get a random duration between 5 and 15 (will add a level multiplier or tree multiplier on this later)
				TotalDuration = FMath::RandRange(MinDuration, MaxDuration);
				
				//Create new instance of a gameplay effect
				
				UGameplayEffect* GameplayEffectInstance = NewObject<UGameplayEffect>(this, PC->WoodcuttingGameplayEffect);
				if (GameplayEffectInstance)
				{
					//Set the gameplay effect duration to total duration (random range)
					GameplayEffectInstance->DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(TotalDuration));
					
					//Set the tree life span (Actor call)
					//Tree->SetLifeSpan(TotalDuration);
					AIdleActorManager* TreeManager = nullptr;

					for (TActorIterator<AIdleActorManager> It(GetWorld()); It; ++It)
					{
						TreeManager = *It;
						break; // Exit the loop once the Tree Manager is found
					}
					//TreeManager->StartTreeCountdown(Tree, TotalDuration);
				}
				
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("WoodcuttingGameplayEffect is nullptr in idleeffectactor"));
				}
				
				//Broadcast to woodcutting ability where the delegate is bound when the tree is clicked on
				PC->CurrentWoodcuttingAbilityInstance->OnTreeLifespanChanged.Broadcast(TotalDuration);
				
				//Get the tree manager for its respawn
				AIdleActorManager* TreeManager = nullptr;

				for (TActorIterator<AIdleActorManager> It(GetWorld()); It; ++It)
				{
					TreeManager = *It;
					break; // Exit the loop once the Tree Manager is found
				}
				//Call cut tree from the tree manager which will set off its respawn as well + a time
				TreeManager->CutTree(Tree);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Woodcutting Ability is Null in idleeffectactor"));
			}
		}
		else 
		{
			UE_LOG(LogTemp, Warning, TEXT("AbilitySystemComponent is nullptr"));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Attempted to set lifespan on a nullptr Tree actor"));
	}
	
}

void AIdleEffectActor::ActivateLegendaryEffect()
{
	if (LegendaryEffect)
	{
		LegendaryEffectParticle->SetAsset(LegendaryEffect);
		LegendaryEffectParticle->Activate();
		//UE_LOG(LogTemp, Warning, TEXT("Activate legendary tree effect in IdleEffectActor"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Legendary effect is null in IdleEffectActor"));
	}
}

void AIdleEffectActor::DeactivateLegendaryEffect()
{
	if (LegendaryEffectParticle)
	{
		LegendaryEffectParticle->Deactivate();
		//UE_LOG(LogTemp, Warning, TEXT("Deactivate legendary tree effect in IdleEffectActor"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Legendary effect particle is null in IdleEffectActor"));
	}
}


