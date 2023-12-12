


#include "Game/SpawnManager.h"
#include "EngineUtils.h"
#include "Components/CapsuleComponent.h"
#include <Kismet/GameplayStatics.h>
#include <PlayFab/PlayFabManager.h>
#include <AbilitySystem/IdleAttributeSet.h>
#include <Player/IdlePlayerState.h>
#include <Player/IdlePlayerController.h>

ASpawnManager* ASpawnManager::SpawnManagerSingletonInstance = nullptr;

ASpawnManager::ASpawnManager()
{
	TreeCount = 0;
	EnemyCount = 0;
	BossCount = 0;
}


void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (!SpawnManagerSingletonInstance)
	{
		SpawnManagerSingletonInstance = this;
	}

    TreeCount = CountActorsWithTag(GetWorld(), "Tree");
    EnemyCount = CountActorsWithTag(GetWorld(), "EnemyPawn");
	BossCount = CountActorsWithTag(GetWorld(), "Boss");

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASpawnManager::BroadcastInitialRunCounts, 2.0f, false);

	InitializeCountdown();

	GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &ASpawnManager::UpdateCountdown, 1.0f, true);

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

void ASpawnManager::UpdateBossCount(int32 Amount)
{
	if (BossCount > 0)
	{
		BossCount -= Amount;
		CheckRunComplete();
		UE_LOG(LogTemp, Warning, TEXT("BossCount updated: %i"), BossCount);
	}	
}

void ASpawnManager::CheckRunComplete()
{
	OnRunCountsUpdated.Broadcast(TreeCount, EnemyCount, BossCount);
	if (TreeCount <= 0 && EnemyCount <= 0 && BossCount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Run Complete"));
		FRunCompleteRewards RewardsToSend;
		FString MapName = UGameplayStatics::GetCurrentLevelName(this, true);
		FString CompletionTime = GetFormattedTime(CountdownTime);

		if (MapName == "EasyMap")
		{
			FRunCompleteRewards EasyRewards;
			EasyRewards.MapDifficulty = "Easy";
			EasyRewards.Time = CompletionTime;
			EasyRewards.Experience = 5000.0f;
			EasyRewards.Wisdom = 100;
			EasyRewards.Temperance = 100;
			EasyRewards.Justice = 100;
			EasyRewards.Courage = 100;
			EasyRewards.LegendaryEssence = 5;
			RewardsToSend = EasyRewards;
		}
		else if (MapName == "MediumMap")
		{
			FRunCompleteRewards MediumRewards;
			MediumRewards.MapDifficulty = "Medium";
			MediumRewards.Time = CompletionTime;
			MediumRewards.Experience = 10000.0f;
			MediumRewards.Wisdom = 150;
			MediumRewards.Temperance = 150;
			MediumRewards.Justice = 150;
			MediumRewards.Courage = 150;
			MediumRewards.LegendaryEssence = 10;
			RewardsToSend = MediumRewards;
		}
		else if (MapName == "HardMap")
		{
			FRunCompleteRewards HardRewards;
			HardRewards.MapDifficulty = "Hard";
			HardRewards.Time = CompletionTime;
			HardRewards.Experience = 15000.0f;
			HardRewards.Wisdom = 200;
			HardRewards.Temperance = 200;
			HardRewards.Justice = 200;
			HardRewards.Courage = 200;
			HardRewards.LegendaryEssence = 15;
			RewardsToSend = HardRewards;
		}
		else if (MapName == "ExpertMap")
		{
			FRunCompleteRewards ExpertRewards;
			ExpertRewards.MapDifficulty = "Expert";
			ExpertRewards.Time = CompletionTime;
			ExpertRewards.Experience = 30000.0f;
			ExpertRewards.Wisdom = 300;
			ExpertRewards.Temperance = 300;
			ExpertRewards.Justice = 300;
			ExpertRewards.Courage = 300;
			ExpertRewards.LegendaryEssence = 30;
			RewardsToSend = ExpertRewards;
		}
		else if (MapName == "LegendaryMap")
		{
			FRunCompleteRewards LegendaryRewards;
			LegendaryRewards.MapDifficulty = "Legendary";
			LegendaryRewards.Time = CompletionTime;
			LegendaryRewards.Experience = 50000.0f;
			LegendaryRewards.Wisdom = 500;
			LegendaryRewards.Temperance = 500;
			LegendaryRewards.Justice = 500;
			LegendaryRewards.Courage = 500;
			LegendaryRewards.LegendaryEssence = 50;
			RewardsToSend = LegendaryRewards;
		}
		else if (MapName == "ImpossibleMap")
		{
			FRunCompleteRewards ImpossibleRewards;
			ImpossibleRewards.MapDifficulty = "Impossible";
			ImpossibleRewards.Time = CompletionTime;
			ImpossibleRewards.Experience = 100000.0f;
			ImpossibleRewards.Wisdom = 1000;
			ImpossibleRewards.Temperance = 1000;
			ImpossibleRewards.Justice = 1000;
			ImpossibleRewards.Courage = 1000;
			ImpossibleRewards.LegendaryEssence = 100;
			RewardsToSend = ImpossibleRewards;
		}

		//Update player rewards on playfab
		APlayFabManager* PlayFabManager = APlayFabManager::GetInstance(GetWorld());
		if (PlayFabManager)
		{
			PlayFabManager->UpdatePlayerRewardsOnPlayFab(RewardsToSend);
		}

		OnRunComplete.Broadcast(RewardsToSend);

		//Update exp, weekly exp, and level
		AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
		AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
		UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);
		IdleAttributeSet->SetWoodcutExp(IdleAttributeSet->GetWoodcutExp() + RewardsToSend.Experience);
		IdleAttributeSet->SetWeeklyWoodcutExp(IdleAttributeSet->GetWeeklyWoodcutExp() + RewardsToSend.Experience);
	}
}

void ASpawnManager::BroadcastInitialRunCounts()
{
	OnRunCountsUpdated.Broadcast(TreeCount, EnemyCount, BossCount);
}

void ASpawnManager::UpdateCountdown()
{
	if (CountdownTime > 0)
	{
		CountdownTime--;
		BroadcastCountdownTime(CountdownTime);
	}
	else
	{
		// Optionally handle what happens when the countdown reaches zero
		GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);

		// Game over screen
		// end the game
	}
}

void ASpawnManager::BroadcastCountdownTime(float Time)
{
	int32 TotalSeconds = static_cast<int32>(Time);
	int32 Minutes = TotalSeconds / 60;
	int32 Seconds = TotalSeconds % 60;

	FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

	// Now broadcast this string to your UI
	OnCountdownUpdated.Broadcast(TimeString);
}

void ASpawnManager::InitializeCountdown()
{
	FString MapName = UGameplayStatics::GetCurrentLevelName(this, true);

	if (MapName == "EasyMap")
	{
		CountdownTime = 1200.0f;
	}
	else if (MapName == "MediumMap")
	{
		CountdownTime = 1800.0f;
	}
	else if (MapName == "HardMap")
	{
		CountdownTime = 2400.0f;
	}
	else if (MapName == "ExpertMap")
	{
		CountdownTime = 3600.0f;
	}
	else if (MapName == "LegendaryMap")
	{
		CountdownTime = 5400.0f;
	}
	else if (MapName == "ImpossibleMap")
	{
		CountdownTime = 7200.0f;
	}
}

FString ASpawnManager::GetFormattedTime(float TimeInSeconds)
{
	int32 TotalSeconds = static_cast<int32>(TimeInSeconds);
	int32 Minutes = TotalSeconds / 60;
	int32 Seconds = TotalSeconds % 60;

	return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
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





