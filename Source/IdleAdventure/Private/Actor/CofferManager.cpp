
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


