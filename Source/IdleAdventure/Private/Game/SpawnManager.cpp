


#include "Game/SpawnManager.h"
#include "EngineUtils.h"
#include "Components/CapsuleComponent.h"

ASpawnManager* ASpawnManager::SpawnManagerSingletonInstance = nullptr;

ASpawnManager::ASpawnManager()
{
	TreeCount = 0;
	EnemyCount = 0;
}


void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (!SpawnManagerSingletonInstance)
	{
		SpawnManagerSingletonInstance = this;
	}

    TreeCount = CountActorsWithTag(GetWorld(), "Tree");
    EnemyCount = CountActorsWithTag(GetWorld(), "Enemy");

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASpawnManager::BroadcastInitialRunCounts, 2.0f, false);

	//SpawnTrees();
	//FTimerHandle SpawnTimerHandle;
	//GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ASpawnManager::SpawnTrees, 2.0f, false);
}

void ASpawnManager::BeginDestroy()
{
	Super::BeginDestroy();

	ResetInstance();
}

ASpawnManager* ASpawnManager::GetInstance(UWorld* World)
{
    if (!SpawnManagerSingletonInstance)
    {
        for (TActorIterator<ASpawnManager> It(World); It; ++It)
        {
            SpawnManagerSingletonInstance = *It;
            break;
        }
        if (!SpawnManagerSingletonInstance)
        {
            SpawnManagerSingletonInstance = World->SpawnActor<ASpawnManager>();
        }
    }
    return SpawnManagerSingletonInstance;
}

void ASpawnManager::ResetInstance()
{
    SpawnManagerSingletonInstance = nullptr;
}

int32 ASpawnManager::CountActorsWithTag(UWorld* World, const FName Tag)
{
	int32 Count = 0;

	if (GetWorld())
	{
		for (TActorIterator<AActor> It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;
			if (Actor)
			{
				// Get the capsule component
				UCapsuleComponent* CapsuleComponent = Actor->FindComponentByClass<UCapsuleComponent>();
				if (CapsuleComponent && CapsuleComponent->ComponentTags.Contains(Tag))
				{
					Count++;
				}
			}
		}
	}

	return Count;
}

void ASpawnManager::UpdateTreeCount(int32 Amount)
{
	if(TreeCount > 0)
	TreeCount -= Amount;
	CheckRunComplete();
	UE_LOG(LogTemp, Warning, TEXT("TreeCount updated: %i"), TreeCount);
}

void ASpawnManager::UpdateEnemyCount(int32 Amount)
{
	if(EnemyCount > 0)
	EnemyCount -= Amount;
	CheckRunComplete();
	UE_LOG(LogTemp, Warning, TEXT("EnemyCount updated: %i"), EnemyCount);
}

void ASpawnManager::CheckRunComplete()
{
	OnRunCountsUpdated.Broadcast(TreeCount, EnemyCount);
	if (TreeCount <= 0 && EnemyCount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Run Complete"));
	}
}

void ASpawnManager::BroadcastInitialRunCounts()
{
	OnRunCountsUpdated.Broadcast(TreeCount, EnemyCount);
}

void ASpawnManager::SpawnTrees()
{
	// Define the spawn parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	// Define the location and rotation for the new actor
	FVector Location = FVector(-26700.0, -11990.0, 894.0); // Change this to your desired location
	FRotator Rotation = FRotator(0.0f, 0.0f, 0.0f); // Change this to your desired rotation

	// Spawn the actor
	AIdleEffectActor* SpawnedActor = GetWorld()->SpawnActor<AIdleEffectActor>(AIdleEffectActor::StaticClass(), Location, Rotation, SpawnParams);

	UE_LOG(LogTemp, Warning, TEXT("Trees Spawned"));
}




