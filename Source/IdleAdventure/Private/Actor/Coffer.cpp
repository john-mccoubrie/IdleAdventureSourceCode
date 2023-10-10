#include "Actor/Coffer.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Player/IdlePlayerState.h>
#include <AbilitySystem/Abilities/ConversionAbility.h>
#include <Actor/CofferManager.h>

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

    //if the coffer is already active, return
    //if there are more than 3 coffers, return
    //broadcast to the UI
    //remove the coffer when its inactive in ondurationeffect removed in conversion ability
    //check essence count for coffer potentially

    //reference to the clicked coffer
    ACoffer* ClickedCoffer = Cast<ACoffer>(CofferHit.GetActor());

    ACofferManager* CofferManager = ACofferManager::GetInstance(GetWorld());
    
    /*
    if (CofferManager->CheckIfCofferIsActive(ClickedCoffer))
    {
        UE_LOG(LogTemp, Warning, TEXT("This coffer is already active"));
        return;
    }
    if (CofferManager->CheckNumOfActiveCoffers() >= 3.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("There are 3 coffers already active"));
        return;
    }
    */

    //UE_LOG(LogTemp, Warning, TEXT("Conditions set, adding coffer and activating conversion ability"));
    CofferManager->ActiveCoffers.Add(ClickedCoffer);
    UE_LOG(LogTemp, Warning, TEXT("Added Coffer: %s"), *ClickedCoffer->GetName());
    AIdlePlayerState* PS = PlayerController->GetPlayerState<AIdlePlayerState>();
    PS->ActivateAbility(UConversionAbility::StaticClass());  
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

