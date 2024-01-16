


#include "Game/SpawnManager.h"
#include "EngineUtils.h"
#include "Components/CapsuleComponent.h"
#include <Kismet/GameplayStatics.h>
#include "AIController.h"
#include "AI/NPCAIController.h"
#include "Character/Enemy_Goblin.h"
#include "Character/Enemy_Goblin_Boss.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"
#include <PlayFab/PlayFabManager.h>
#include <AbilitySystem/IdleAttributeSet.h>
#include <Player/IdlePlayerState.h>
#include <Player/IdlePlayerController.h>
#include <Game/SteamManager.h>
#include <Save/IdleSaveGame.h>

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

	InitializeMapDifficulty();
	LoadCompletedLevels();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASpawnManager::BroadcastInitialRunCounts, 2.0f, false);

	InitializeCountdown();

	GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &ASpawnManager::UpdateCountdown, 1.0f, true);

	/*
	if (SteamAPI_Init()) {
		// Steam API is initialized
	}
	else {
		// Handle error, Steam API not available
	}
	*/
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
	ASteamManager* SteamManager = ASteamManager::GetInstance(GetWorld());
	OnRunCountsUpdated.Broadcast(TreeCount, EnemyCount, BossCount);
	if (TreeCount <= 0 && EnemyCount <= 0 && BossCount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Run Complete"));
		FString MapName = UGameplayStatics::GetCurrentLevelName(this, true);
		FString CompletionTime = GetFormattedTime(CountdownTime);

		//Create or load save game
		UIdleSaveGame* SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("CompletedLevelsSaveSlot", 0));
		if (!SaveGameInstance)
		{
			SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::CreateSaveGameObject(UIdleSaveGame::StaticClass()));
		}

		if (MapName == "EasyMap")
		{
			FRunCompleteRewards EasyRewards;
			EasyRewards.MapDifficulty = "Easy";
			EasyRewards.Time = CompletionTime;
			EasyRewards.Experience = 5000.0f;
			EasyRewards.Wisdom = 100;
			EasyRewards.Temperance = 50;
			EasyRewards.Justice = 30;
			EasyRewards.Courage = 20;
			EasyRewards.LegendaryEssence = 2;
			RewardsToSend = EasyRewards;
			if (SteamManager)
			{
				SteamManager->UnlockSteamAchievement(TEXT("COMPLETE_EASY"));
			}

			SaveCompletedLevel(EasyRewards.MapDifficulty);
		}
		else if (MapName == "MediumMap")
		{
			FRunCompleteRewards MediumRewards;
			MediumRewards.MapDifficulty = "Medium";
			MediumRewards.Time = CompletionTime;
			MediumRewards.Experience = 10000.0f;
			MediumRewards.Wisdom = 150;
			MediumRewards.Temperance = 70;
			MediumRewards.Justice = 50;
			MediumRewards.Courage = 30;
			MediumRewards.LegendaryEssence = 5;
			RewardsToSend = MediumRewards;
			if (SteamManager)
			{
				SteamManager->UnlockSteamAchievement(TEXT("COMPLETE_MEDIUM"));
			}

			SaveCompletedLevel(MediumRewards.MapDifficulty);
		}
		else if (MapName == "HardMap")
		{
			FRunCompleteRewards HardRewards;
			HardRewards.MapDifficulty = "Hard";
			HardRewards.Time = CompletionTime;
			HardRewards.Experience = 20000.0f;
			HardRewards.Wisdom = 200;
			HardRewards.Temperance = 100;
			HardRewards.Justice = 50;
			HardRewards.Courage = 30;
			HardRewards.LegendaryEssence = 7;
			RewardsToSend = HardRewards;
			if (SteamManager)
			{
				SteamManager->UnlockSteamAchievement(TEXT("COMPLETE_HARD"));
			}

			SaveCompletedLevel(HardRewards.MapDifficulty);
		}
		else if (MapName == "ExpertMap")
		{
			FRunCompleteRewards ExpertRewards;
			ExpertRewards.MapDifficulty = "Expert";
			ExpertRewards.Time = CompletionTime;
			ExpertRewards.Experience = 30000.0f;
			ExpertRewards.Wisdom = 300;
			ExpertRewards.Temperance = 200;
			ExpertRewards.Justice = 100;
			ExpertRewards.Courage = 50;
			ExpertRewards.LegendaryEssence = 10;
			RewardsToSend = ExpertRewards;
			if (SteamManager)
			{
				SteamManager->UnlockSteamAchievement(TEXT("COMPLETE_EXPERT"));
			}

			SaveCompletedLevel(ExpertRewards.MapDifficulty);
		}
		else if (MapName == "LegendaryMap")
		{
			FRunCompleteRewards LegendaryRewards;
			LegendaryRewards.MapDifficulty = "Legendary";
			LegendaryRewards.Time = CompletionTime;
			LegendaryRewards.Experience = 50000.0f;
			LegendaryRewards.Wisdom = 400;
			LegendaryRewards.Temperance = 250;
			LegendaryRewards.Justice = 150;
			LegendaryRewards.Courage = 100;
			LegendaryRewards.LegendaryEssence = 12;
			RewardsToSend = LegendaryRewards;
			if (SteamManager)
			{
				SteamManager->UnlockSteamAchievement(TEXT("COMPLETE_LEGENDARY"));
			}

			SaveCompletedLevel(LegendaryRewards.MapDifficulty);
		}
		else if (MapName == "ImpossibleMap")
		{
			FRunCompleteRewards ImpossibleRewards;
			ImpossibleRewards.MapDifficulty = "Impossible";
			ImpossibleRewards.Time = CompletionTime;
			ImpossibleRewards.Experience = 100000.0f;
			ImpossibleRewards.Wisdom = 500;
			ImpossibleRewards.Temperance = 300;
			ImpossibleRewards.Justice = 200;
			ImpossibleRewards.Courage = 100;
			ImpossibleRewards.LegendaryEssence = 15;
			RewardsToSend = ImpossibleRewards;
			if (SteamManager)
			{
				SteamManager->UnlockSteamAchievement(TEXT("COMPLETE_IMPOSSIBLE"));
			}

			SaveCompletedLevel(ImpossibleRewards.MapDifficulty);
		}
		else if (MapName == "TutorialMap")
		{
			FRunCompleteRewards TutorialRewards;
			TutorialRewards.MapDifficulty = "Tutorial";
			TutorialRewards.Time = CompletionTime;
			TutorialRewards.Experience = 500.0f;
			TutorialRewards.Wisdom = 10;
			TutorialRewards.Temperance = 10;
			TutorialRewards.Justice = 10;
			TutorialRewards.Courage = 10;
			TutorialRewards.LegendaryEssence = 1;
			RewardsToSend = TutorialRewards;
			if (SteamManager)
			{
				SteamManager->UnlockSteamAchievement(TEXT("COMPLETE_TUTORIAL"));
			}

			SaveCompletedLevel(TutorialRewards.MapDifficulty);
		}
		else if (MapName == "GatheringMap")
		{
			FRunCompleteRewards GatheringRewards;
			GatheringRewards.MapDifficulty = "Gathering";
			GatheringRewards.Time = CompletionTime;
			GatheringRewards.Experience = 10000.0f;
			GatheringRewards.Wisdom = 100;
			GatheringRewards.Temperance = 50;
			GatheringRewards.Justice = 20;
			GatheringRewards.Courage = 10;
			GatheringRewards.LegendaryEssence = 3;
			RewardsToSend = GatheringRewards;
			if (SteamManager)
			{
				SteamManager->UnlockSteamAchievement(TEXT("COMPLETE_GATHERING"));
			}

			SaveCompletedLevel(GatheringRewards.MapDifficulty);
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

		//Play run complete sound
		PC->IdleInteractionComponent->PlayRunCompleteSound();
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
	//else if sounds for every 300 seconds or so
	else
	{
		// Optionally handle what happens when the countdown reaches zero
		int32 TotalSeconds = static_cast<int32>(CountdownTime);
		int32 Minutes = TotalSeconds / 60;
		int32 Seconds = TotalSeconds % 60;

		FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
		OnTimerAtZero.Broadcast(TimeString, RewardsToSend.MapDifficulty, "Time Expired",
			"Visit the StoicStore to purchase auras that grant speed bonuses, staves that grant attack bonuses, both which help gather resources and kill enemies faster. Also, try not being so slow."
		);;
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
	else if (MapName == "TutorialMap")
	{
		CountdownTime = 3600.0f;
	}
	else if (MapName == "GatheringMap")
	{
		CountdownTime = 3600.0f;
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

void ASpawnManager::InitializeMapDifficulty()
{
	FString CurrentMapName = UGameplayStatics::GetCurrentLevelName(this, true);

	if (CurrentMapName == "EasyMap")
	{
		RewardsToSend.MapDifficulty = "Easy";
	}
	else if (CurrentMapName == "MediumMap")
	{
		RewardsToSend.MapDifficulty = "Medium";
	}
	else if (CurrentMapName == "HardMap")
	{
		RewardsToSend.MapDifficulty = "Hard";
	}
	else if (CurrentMapName == "ExpertMap")
	{
		RewardsToSend.MapDifficulty = "Expert";
	}
	else if (CurrentMapName == "LegendaryMap")
	{
		RewardsToSend.MapDifficulty = "Legendary";
	}
	else if (CurrentMapName == "ImpossibleMap")
	{
		RewardsToSend.MapDifficulty = "Impossible";
	}
	else if (CurrentMapName == "TutorialMap")
	{
		RewardsToSend.MapDifficulty = "Tutorial";
	}
	else if (CurrentMapName == "GatheringMap")
	{
		RewardsToSend.MapDifficulty = "Gathering";
	}
	else
	{
		// Default or unknown map
		RewardsToSend.MapDifficulty = "Unknown";
	}
	InitialRunDataSet.Broadcast(RewardsToSend.MapDifficulty);
}

void ASpawnManager::StopCountdownTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
}

void ASpawnManager::ScheduleRespawn(FString EnemyType, TSubclassOf<AEnemyBase> TutorialGoblinReference, FVector Location, FRotator Rotation)
{
	FTimerHandle TimerHandle;
	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &ASpawnManager::RespawnEnemy, EnemyType, TutorialGoblinReference, Location, Rotation);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, RespawnDelegate, 5.0f, false);
}

void ASpawnManager::RespawnEnemy(FString EnemyType, TSubclassOf<AEnemyBase> TutorialGoblinReference, FVector Location, FRotator Rotation)
{
	UWorld* World = GetWorld();
	if (World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Made it to respawn enemy"));

		if (EnemyType == "Goblin")
		{
			// Use the provided Location instead of hardcoding it
			FVector StartLocation(-18232.3814f, -11854.042262f, 1022.553947f);
			// Spawn the enemy goblin using the Blueprint reference
			AEnemy_Goblin* SpawnedGoblin = World->SpawnActor<AEnemy_Goblin>(TutorialGoblinReference, StartLocation, Rotation);
			// Log the spawning
			UE_LOG(LogTemp, Warning, TEXT("Spawned tutorial goblin at Location: %s, Rotation: %s"),
				*SpawnedGoblin->GetActorLocation().ToString(), *SpawnedGoblin->GetActorRotation().ToString());

			// Check if the Goblin has been possessed by an AI Controller
			if (SpawnedGoblin->AIControllerClass && !SpawnedGoblin->GetController())
			{



				// Spawn the AI Controller
				auto AIController = World->SpawnActor<ANPCAIController>(SpawnedGoblin->AIControllerClass, Location, Rotation);

				if (AIController)
				{
					// Possess the spawned Goblin with the new AI Controller
					AIController->Possess(SpawnedGoblin);

					// If the Goblin has a Behavior Tree associated with it, start the Behavior Tree
					if (AIController->GetBlackboardComponent() && SpawnedGoblin->BehaviorTree)
					{
						//AIController->GetBlackboardComponent()->InitializeBlackboard(*(SpawnedGoblin->BehaviorTree->BlackboardAsset));
						//AIController->RunBehaviorTree(SpawnedGoblin->BehaviorTree);
					}
				}
			}
		}
		else
		{
			FVector BossStartLocation(-20666.537117, -31360.737458, 1659.101517);
			AEnemy_Goblin_Boss* SpawnedBoss = World->SpawnActor<AEnemy_Goblin_Boss>(TutorialGoblinReference, BossStartLocation, Rotation);
			// Log the spawning
			UE_LOG(LogTemp, Warning, TEXT("Spawned tutorial goblin at Location: %s, Rotation: %s"),
				*SpawnedBoss->GetActorLocation().ToString(), *SpawnedBoss->GetActorRotation().ToString());

			// Check if the Goblin has been possessed by an AI Controller
			if (SpawnedBoss->AIControllerClass && !SpawnedBoss->GetController())
			{

				// Spawn the AI Controller
				auto AIController = World->SpawnActor<ANPCAIController>(SpawnedBoss->AIControllerClass, Location, Rotation);

				if (AIController)
				{
					// Possess the spawned Goblin with the new AI Controller
					AIController->Possess(SpawnedBoss);

					// If the Goblin has a Behavior Tree associated with it, start the Behavior Tree
					if (AIController->GetBlackboardComponent() && SpawnedBoss->BehaviorTree)
					{
						//AIController->GetBlackboardComponent()->InitializeBlackboard(*(SpawnedGoblin->BehaviorTree->BlackboardAsset));
						//AIController->RunBehaviorTree(SpawnedGoblin->BehaviorTree);
					}
				}
			}
		}			
	}
}

void ASpawnManager::SaveCompletedLevel(const FString& LevelName)
{
	// Retrieve an existing save game or create a new one if it doesn't exist
	UIdleSaveGame* SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("CompletedLevelsSaveSlot", 0));
	if (!SaveGameInstance)
	{
		SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::CreateSaveGameObject(UIdleSaveGame::StaticClass()));
	}

	// Save the completed level
	SaveGameInstance->SaveCompletedLevel(LevelName);
}

void ASpawnManager::LoadCompletedLevels()
{
	// Retrieve an existing save game
	UIdleSaveGame* LoadGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("CompletedLevelsSaveSlot", 0));
	if (LoadGameInstance)
	{
		// Broadcast the array of completed levels
		FOnLoadCompletedLevels.Broadcast(LoadGameInstance->LoadCompletedLevels());
	}
	else
	{
		// Broadcast an empty array if no save game is found
		FOnLoadCompletedLevels.Broadcast(TArray<FString>());
	}
}






