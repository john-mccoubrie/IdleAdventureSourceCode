#include "Actor/Coffer.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Player/IdlePlayerState.h>
#include <AbilitySystem/Abilities/ConversionAbility.h>

ACoffer::ACoffer()
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;
}

void ACoffer::CofferClicked(FHitResult CofferHit)
{
   
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController) return;

    //Range between the player and the tree (if it's clickable)
    float InRange = 800.f;
    APawn* PlayerPawn = PlayerController->GetPawn();
    if (!PlayerPawn) return;

    //Calculate the current distance between the player and the tree
    FVector PlayerLocation = PlayerPawn->GetActorLocation();
    FVector ObjectLocation = CofferHit.GetActor()->GetActorLocation();
    float Distance = FVector::Distance(PlayerLocation, ObjectLocation);

    //reference to the clicked coffer
    ACoffer* ClickedCoffer = Cast<ACoffer>(CofferHit.GetActor());

    //if the clicked coffer is in range grant the conversion ability
    if (ClickedCoffer && Distance <= InRange)
    {
        AIdlePlayerState* PS = PlayerController->GetPlayerState<AIdlePlayerState>();
        PS->ActivateAbility(UConversionAbility::StaticClass());
    }
}

// Starts the timer on the coffer, decrementing by 1 every second based on the conversion abilities gameplay effect duration
void ACoffer::StartExperienceTimer(float Duration)
{
    //UE_LOG(LogTemp, Warning, TEXT("StartExperienceTimer called"));
    TotalExperienceTime = Duration;
    RemainingExperienceTime = Duration;
    GetWorld()->GetTimerManager().SetTimer(ExperienceTimerHandle, this, &ACoffer::DecrementExperienceTime, 1.0f, true);
}

void ACoffer::DecrementExperienceTime()
{
    //UE_LOG(LogTemp, Warning, TEXT("DecrementExperienceTimer called"));
    RemainingExperienceTime -= 1.0f;
    //broadcasts the decreasing timer to the UI elements
    OnCofferExpCount.Broadcast(RemainingExperienceTime / TotalExperienceTime);
    if (RemainingExperienceTime <= 0.0f)
    {
        GetWorld()->GetTimerManager().ClearTimer(ExperienceTimerHandle);
    }
}
