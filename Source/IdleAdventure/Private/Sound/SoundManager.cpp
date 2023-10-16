
#include "Sound/SoundManager.h"
#include "EngineUtils.h"
#include <Kismet/GameplayStatics.h>

ASoundManager* ASoundManager::SoundManagerSingletonInstance = nullptr;
// Sets default values
ASoundManager::ASoundManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASoundManager::BeginPlay()
{
	Super::BeginPlay();

    PlayRandomSound();

	if (!SoundManagerSingletonInstance)
	{
		SoundManagerSingletonInstance = this;
	}
	
}

void ASoundManager::BeginDestroy()
{
    Super::BeginDestroy();

    ResetInstance();
}

ASoundManager* ASoundManager::GetInstance(UWorld* World)
{
    if (!SoundManagerSingletonInstance)
    {
        for (TActorIterator<ASoundManager> It(World); It; ++It)
        {
            SoundManagerSingletonInstance = *It;
            break;
        }
        if (!SoundManagerSingletonInstance)
        {
            SoundManagerSingletonInstance = World->SpawnActor<ASoundManager>();
        }
    }
    return SoundManagerSingletonInstance;
}

void ASoundManager::ResetInstance()
{
    SoundManagerSingletonInstance = nullptr;
}

// Called every frame
void ASoundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASoundManager::PlayRandomSound()
{
    if (SoundCues.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, SoundCues.Num() - 1);
        USoundCue* SelectedSoundCue = SoundCues[RandomIndex];

        UGameplayStatics::PlaySoundAtLocation(this, SelectedSoundCue, GetActorLocation());

        // Set a timer to play the next sound after the current one finishes
        GetWorldTimerManager().SetTimer(SoundTimerHandle, this, &ASoundManager::PlayRandomSound, SelectedSoundCue->GetDuration(), false);
    }
}

