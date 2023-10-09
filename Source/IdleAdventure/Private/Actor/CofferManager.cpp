
#include "Actor/CofferManager.h"
#include "EngineUtils.h"

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


