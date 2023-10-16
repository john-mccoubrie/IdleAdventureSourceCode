
#include "Actor/CofferManager.h"
#include "EngineUtils.h"
#include <Kismet/GameplayStatics.h>

ACofferManager* ACofferManager::CofferManagerSingletonInstance = nullptr;

// Sets default values
ACofferManager::ACofferManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACofferManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (!CofferManagerSingletonInstance)
	{
		CofferManagerSingletonInstance = this;
	}

}

void ACofferManager::BeginDestroy()
{
	Super::BeginDestroy();
	
	ResetInstance();
}

ACofferManager* ACofferManager::GetInstance(UWorld* World)
{
    if (!CofferManagerSingletonInstance)
    {
        for (TActorIterator<ACofferManager> It(World); It; ++It)
        {
            CofferManagerSingletonInstance = *It;
            break;
        }
        if (!CofferManagerSingletonInstance)
        {
            CofferManagerSingletonInstance = World->SpawnActor<ACofferManager>();
        }
    }
    return CofferManagerSingletonInstance;
}

void ACofferManager::ResetInstance()
{
    CofferManagerSingletonInstance = nullptr;
}

void ACofferManager::SetUpAllCoffers()
{
    // Clear the existing list of all coffers (if any)
    AllCoffers.Empty();

    // Temporary array to hold AActor references
    TArray<AActor*> TempCoffers;

    // Get all ACoffer instances
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACoffer::StaticClass(), TempCoffers);

    // Cast AActor references to ACoffer and assign to AllCoffers
    for (AActor* Actor : TempCoffers)
    {
        if (ACoffer* Coffer = Cast<ACoffer>(Actor))
        {
            AllCoffers.Add(Coffer);
        }
    }
}

bool ACofferManager::CheckIfCofferIsActive(ACoffer* CofferToCheck)
{
    if (!CofferToCheck)
    {
        return false;  // CofferToCheck is null
    }

    // Return true if the CofferToCheck is present in the ActiveCoffers array
    return ActiveCoffers.Contains(CofferToCheck);
}

int32 ACofferManager::CheckNumOfActiveCoffers()
{
    return ActiveCoffers.Num();
}

void ACofferManager::AddActiveCoffer(ACoffer* NewCoffer)
{
    if (ActiveCoffers.Num() < 3)
    {
        ActiveCoffers.Add(NewCoffer);
        OnActiveCofferCountChanged.Broadcast(NewCoffer);
    }
}

void ACofferManager::RemoveActiveCoffer(ACoffer* CofferToRemove)
{
    ActiveCoffers.Remove(CofferToRemove);
    if (CofferProgressBarMapping.Contains(CofferToRemove))
    {
        CofferProgressBarMapping.Remove(CofferToRemove);
    }
    //OnActiveCofferCountChanged.Broadcast(ActiveCoffers.Num());
}

void ACofferManager::UpdateProgressBar(ACoffer* UpdatedCoffer, float ProgressRatio)
{
    //if (CofferProgressBarMapping.Contains(UpdatedCoffer))
    //{
        //int32 ProgressBarID = CofferProgressBarMapping[UpdatedCoffer];
    //OnCofferClicked.Clear();
    UE_LOG(LogTemp, Error, TEXT("ProgressRatio: %f"), ProgressRatio);
    //OnCofferClicked.Broadcast(UpdatedCoffer, ProgressRatio);
    //}
}

void ACofferManager::StartExperienceTimer(float Duration)
{
    // If a timer is already active, simply add to the remaining time
    if (GetWorld()->GetTimerManager().IsTimerActive(ExperienceTimerHandle))
    {
        RemainingExperienceTime += Duration;
        //TotalExperienceTime += Duration;
    }
    else // If no timer is active, set the values fresh
    {
        UE_LOG(LogTemp, Error, TEXT("Duration: %f"), Duration);
        TotalExperienceTime = 500;
        RemainingExperienceTime = Duration;
        UE_LOG(LogTemp, Error, TEXT("TotalExperienceTime: %f"), TotalExperienceTime);
        UE_LOG(LogTemp, Error, TEXT("RemainingExperienceTime: %f"), TotalExperienceTime);

        // Start a new timer
        GetWorld()->GetTimerManager().SetTimer(ExperienceTimerHandle, this, &ACofferManager::DecrementExperienceTime, 1.0f, true);
    }
}

void ACofferManager::DecrementExperienceTime()
{
    RemainingExperienceTime -= 1.0f;
    float progress = FMath::Clamp(RemainingExperienceTime / TotalExperienceTime, 0.0f, 1.0f); // Clamp the value
    UE_LOG(LogTemp, Error, TEXT("Progress: %f"), progress);
    OnCofferClicked.Broadcast(progress);

    if (RemainingExperienceTime <= 0.0f)
    {
        GetWorld()->GetTimerManager().ClearTimer(ExperienceTimerHandle);
    }
}


