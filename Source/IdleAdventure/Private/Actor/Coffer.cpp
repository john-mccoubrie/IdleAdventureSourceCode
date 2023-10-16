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

    ClickedCoffer = Cast<ACoffer>(CofferHit.GetActor());

    ACofferManager* CofferManager = ACofferManager::GetInstance(GetWorld());
    CofferManager->AddActiveCoffer(ClickedCoffer);
  
    AIdlePlayerState* PS = PlayerController->GetPlayerState<AIdlePlayerState>();
    PS->ActivateAbility(UConversionAbility::StaticClass());  
}

/*
void ACoffer::StartExperienceTimer(float Duration)
{
    // If a timer is already active, simply add to the remaining time
    if (GetWorld()->GetTimerManager().IsTimerActive(ExperienceTimerHandle))
    {
        RemainingExperienceTime += Duration;
    }
    else // If no timer is active, set the values fresh
    {
        UE_LOG(LogTemp, Error, TEXT("Duration: %f"), Duration);
        TotalExperienceTime = Duration;
        RemainingExperienceTime = Duration;
        UE_LOG(LogTemp, Error, TEXT("TotalExperienceTime: %f"), TotalExperienceTime);
        UE_LOG(LogTemp, Error, TEXT("RemainingExperienceTime: %f"), TotalExperienceTime);

        // Start a new timer
        GetWorld()->GetTimerManager().SetTimer(ExperienceTimerHandle, this, &ACoffer::DecrementExperienceTime, 1.0f, true);
    }
}
*/



/*
void ACoffer::DecrementExperienceTime()
{
    //UE_LOG(LogTemp, Warning, TEXT("DecrementExperienceTimer called"));
    ACofferManager* CofferManager = ACofferManager::GetInstance(GetWorld());
    RemainingExperienceTime -= 1.0f;
    //broadcasts the decreasing timer to the UI elements
    //OnCofferExpCount.Broadcast(RemainingExperienceTime / TotalExperienceTime);
    CofferManager->UpdateProgressBar(this, RemainingExperienceTime / TotalExperienceTime);
    if (RemainingExperienceTime <= 0.0f)
    {
        //CofferManager->ProgressBarIndex--; this is handled in the manager now
        //UE_LOG(LogTemp, Warning, TEXT("ProgressBarIndex equals: %i"), CofferManager->ProgressBarIndex);
        //Remove ClickedCoffer from array?
        CofferManager->RemoveActiveCoffer(ClickedCoffer);
        GetWorld()->GetTimerManager().ClearTimer(ExperienceTimerHandle);
    }
}
*/
void ACoffer::AddExperienceTime(float AdditionalTime)
{
    //UE_LOG(LogTemp, Warning, TEXT("Additional Time Called in AddExp: %f"), AdditionalTime);
    //UE_LOG(LogTemp, Warning, TEXT("Total Time Called in AddExp: %f"), TotalExperienceTime);
    ACofferManager* CofferManager = ACofferManager::GetInstance(GetWorld());
    //GetWorld()->GetTimerManager().ClearTimer(ExperienceTimerHandle);
    //TotalExperienceTime = RemainingExperienceTime + AdditionalTime;
    //RemainingExperienceTime += AdditionalTime;
    //CofferManager->UpdateProgressBar(this, RemainingExperienceTime / TotalExperienceTime);
}

