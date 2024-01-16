#include "Player/IdlePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/IdleAbilitySystemComponent.h"
#include "AbilitySystem/IdleAttributeSet.h"
#include <Game/IdleGameModeBase.h>
//#include <Leaderboard/LeaderboardManager.h>
#include <AbilitySystem/Abilities/WoodcuttingAbility.h>
#include <UI/HUD/IdleHUD.h>
#include <Player/IdlePlayerController.h>
#include <UI/IdleWidgetController.h>
#include <Leaderboard/PlayLoginActor.h>
#include <Kismet/GameplayStatics.h>
#include <Save/IdleSaveGame.h>
#include <PlayerEquipment/EquipmentManager.h>


AIdlePlayerState::AIdlePlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UIdleAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UIdleAttributeSet>("AttributeSet");

	NetUpdateFrequency = 100.f;
	UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(AttributeSet);

	//LeaderboardManager = CreateDefaultSubobject<ULeaderboardManager>("LeaderboardManager");

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		IdleAttributeSet->GetWoodcutExpAttribute()).AddUObject(this, &AIdlePlayerState::CheckForLevelUp);

}

void AIdlePlayerState::BeginPlay()
{
	//stop level up effects from firing during game set up
	bShouldTriggerLevelUpEffects = false;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("LoginActorTag"), FoundActors);
	APlayLoginActor* MyPlayLoginActor = Cast<APlayLoginActor>(FoundActors[0]);
	MyPlayLoginActor->LoadWoodcuttingExpFromPlayFab();
	LoadExp();
	GetWorld()->GetTimerManager().SetTimer(SaveGameTimerHandle, this, &AIdlePlayerState::AutoSaveGame, 20.f, true);

	
	//Set initial values
	//InitializePlayerValues();
}


UAbilitySystemComponent* AIdlePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AIdlePlayerState::GiveAbility(const FGameplayAbilityInfo& AbilityInfo)
{
	//UE_LOG(LogTemp, Warning, TEXT("GIVE ABILITY PS"));
	FGameplayAbilitySpec Spec(AbilityInfo.Ability, AbilityInfo.Level, AbilityInfo.InputID, this);
	AbilitySystemComponent->GiveAbility(Spec);
	AbilityHandles.Add(AbilityInfo.Ability, Spec.Handle);
	GrantedAbilities.Add(AbilityInfo);
}

void AIdlePlayerState::ActivateAbility(TSubclassOf<UGameplayAbility> AbilityToActivate)
{
	FGameplayAbilitySpecHandle* FoundHandle = AbilityHandles.Find(AbilityToActivate);
	if (FoundHandle)
	{
		AbilitySystemComponent->TryActivateAbility(*FoundHandle);
		//UE_LOG(LogTemp, Warning, TEXT("ACTIVATE ABILITY PS"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability not found in player state"));
	}
}

void AIdlePlayerState::DeactivateAbility(TSubclassOf<UGameplayAbility> AbilityToDeactivate)
{
	FGameplayAbilitySpecHandle* FoundHandle = AbilityHandles.Find(AbilityToDeactivate);
	if (FoundHandle)
	{
		// Get the spec using the handle
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(*FoundHandle);

		// Check if we got a valid spec
		if (AbilitySpec && AbilitySpec->IsActive())
		{
			// Get the primary active instance (if there are multiple instances of this ability, you might need more complex logic)
			UGameplayAbility* ActiveAbility = AbilitySpec->GetPrimaryInstance();

			// Cancel the ability
			if (ActiveAbility)
			{
				ActiveAbility->CancelAbility(*FoundHandle, ActiveAbility->GetCurrentActorInfo(), ActiveAbility->GetCurrentActivationInfo(), true); // Assuming you want to replicate
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability not found in player state"));
	}
}

bool AIdlePlayerState::IsAbilityActive(TSubclassOf<UGameplayAbility> AbilityClass)
{
	FGameplayAbilitySpecHandle* FoundHandle = AbilityHandles.Find(AbilityClass);
	if (FoundHandle)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void AIdlePlayerState::InitializePlayerValues()
{
	UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(AttributeSet);
	OnExpGained.Broadcast(IdleAttributeSet->GetWoodcutExp());
;	OnExpGained.Broadcast(IdleAttributeSet->GetWeeklyWoodcutExp());
	WhenLevelUp.Broadcast(IdleAttributeSet->GetWoodcuttingLevel());
}

void AIdlePlayerState::CheckForLevelUp(const FOnAttributeChangeData& Data) const
{
	UE_LOG(LogTemp, Warning, TEXT("CheckForLevelUp"));

	// Adjusted growth rates and transition level
	float earlyGrowthRate = 1.6f;
	float lateGrowthRate = 1.2f;
	int transitionLevel = 7;
	float initialExpForLevelOne = 2000.0f;

	UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(AttributeSet);

	// Player's total experience
	float totalExp = IdleAttributeSet->GetWoodcutExp();
	int currentLevel = IdleAttributeSet->GetWoodcuttingLevel();

	// Calculate the experience for the current and next level
	float expForCurrentLevel = (currentLevel == 0) ? 0.0f : initialExpForLevelOne;
	float expForNextLevel = initialExpForLevelOne;

	for (int lvl = 1; lvl <= currentLevel; ++lvl)
	{
		float growthRate = (lvl < transitionLevel) ? earlyGrowthRate : lateGrowthRate;
		expForCurrentLevel = expForNextLevel;
		expForNextLevel *= growthRate;
	}

	// Experience relative to the current level
	float relativeExp = totalExp - expForCurrentLevel;

	// Progress as a percentage
	float progress = (relativeExp / (expForNextLevel - expForCurrentLevel)) * 100;

	// Broadcast the current progress
	WhenLevelUp.Broadcast(progress);
	ShowExpNumbersOnText.Broadcast(IdleAttributeSet->GetWoodcutExp(), expForNextLevel);
	ShowPlayerLevelOnText.Broadcast(IdleAttributeSet->GetWoodcuttingLevel());

	// If the player has leveled up
	while (IdleAttributeSet->GetWoodcutExp() >= expForNextLevel)
	{
		UE_LOG(LogTemp, Warning, TEXT("Level up"));

		// Increase the level
		IdleAttributeSet->SetWoodcuttingLevel(IdleAttributeSet->GetWoodcuttingLevel() + 1.f);
		currentLevel = IdleAttributeSet->GetWoodcuttingLevel();

		// Recalculate expForCurrentLevel and expForNextLevel for the new level
		float growthRate = (currentLevel < transitionLevel) ? earlyGrowthRate : lateGrowthRate;
		expForCurrentLevel = expForNextLevel;
		expForNextLevel *= growthRate;

		relativeExp = totalExp - expForCurrentLevel;
		progress = (relativeExp / (expForNextLevel - expForCurrentLevel)) * 100;

		// Additional logic for when the player levels up
		// ... (Play sounds, update UI, etc.) ...

		// Broadcast level up delegate
		WhenLevelUp.Broadcast(progress);
		ShowExpNumbersOnText.Broadcast(IdleAttributeSet->GetWoodcutExp(), expForNextLevel);
		ShowPlayerLevelOnText.Broadcast(IdleAttributeSet->GetWoodcuttingLevel());

		AIdlePlayerController* PlayerController = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());

		if (bShouldTriggerLevelUpEffects)
		{
			//PlayLevel up sound
			PlayerController->IdleInteractionComponent->PlayLevelUpSound();
			//PlayLevelUp particle effect
			APawn* MyPawn = PlayerController->GetPawn();
			PlayerController->IdleInteractionComponent->SpawnLevelUpEffect(MyPawn);

			//Game chat notification
			AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
			int WoodcuttingLevelAsInt = static_cast<int>(IdleAttributeSet->GetWoodcuttingLevel());
			FString formattedMessage = FString::Printf(TEXT("Congratulations, you leveled up! You are now level %d"), WoodcuttingLevelAsInt);
			GameChatManager->PostNotificationToUI(formattedMessage, FLinearColor::Yellow);
		}

	}

	bShouldTriggerLevelUpEffects = true;
}


void AIdlePlayerState::UpdateXPThreshold()
{
	UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(AttributeSet);
	IdleAttributeSet->SetXPToNextLevel(IdleAttributeSet->GetXPToNextLevel() + 100);
}

void AIdlePlayerState::AutoSaveGame()
{
	UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(AttributeSet);
	FString PlayerName = TEXT("PlayerName");

	// Calculate weekly experience (you'll need to define this based on your game's logic)
	//int32 WeeklyExp = CalculateWeeklyExp(IdleAttributeSet->GetWoodcutExp());

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("LoginActorTag"), FoundActors);
	if (FoundActors.Num() > 0)
	{
		APlayLoginActor* MyPlayLoginActor = Cast<APlayLoginActor>(FoundActors[0]);
		if (MyPlayLoginActor)
		{
			// Update both global and weekly experience values on PlayFab
			//UE_LOG(LogTemp, Warning, TEXT("GlobalExp in PC: %f"), IdleAttributeSet->GetWoodcutExp());
			//UE_LOG(LogTemp, Warning, TEXT("WeeklyExp in PC: %f"), IdleAttributeSet->GetWeeklyWoodcutExp());
			MyPlayLoginActor->SavePlayerStatsToPlayFab(PlayerName, IdleAttributeSet->GetWoodcutExp(), IdleAttributeSet->GetWeeklyWoodcutExp(), IdleAttributeSet->GetWoodcuttingLevel());
		}
	}
}

void AIdlePlayerState::LoadExp()
{
	//UE_LOG(LogTemp, Warning, TEXT("LOAD EXP CALLED"));
	//Load values from playfab
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("LoginActorTag"), FoundActors);
		if (FoundActors.Num() > 0)
		{
			APlayLoginActor* MyPlayLoginActor = Cast<APlayLoginActor>(FoundActors[0]);
			if (MyPlayLoginActor)
			{
				// Ensure we only bind once
				if (!bHasBoundToPlayFabDelegate)
				{
					MyPlayLoginActor->OnExpLoaded.AddDynamic(this, &AIdlePlayerState::OnPlayFabExpLoaded);
					MyPlayLoginActor->OnPlayerLevelLoaded.AddDynamic(this, &AIdlePlayerState::OnPlayFabPlayerLevelLoaded);
					MyPlayLoginActor->OnWeeklyExpLoaded.AddDynamic(this, &AIdlePlayerState::OnPlayFabWeeklyExpLoaded);
					bHasBoundToPlayFabDelegate = true;
				}

				// Now, request PlayFab to load the EXP and Player Level
				MyPlayLoginActor->LoadWoodcuttingExpFromPlayFab();
			}
		}
	
}

void AIdlePlayerState::OnPlayFabExpLoaded(int32 LoadedExp)
{
	int32 FinalExp = FMath::Max(LocalWoodcuttingExp, LoadedExp);

	UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(AttributeSet);
	IdleAttributeSet->SetWoodcutExp(LoadedExp);
	//UE_LOG(LogTemp, Warning, TEXT("Global Exp set in PC: %f"), IdleAttributeSet->GetWoodcutExp());
}

void AIdlePlayerState::OnPlayFabWeeklyExpLoaded(int32 LoadedWeeklyExp)
{
	int32 FinalExp = FMath::Max(LocalWeeklyWoodcuttingExp, LoadedWeeklyExp);

	UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(AttributeSet);
	IdleAttributeSet->SetWeeklyWoodcutExp(LoadedWeeklyExp);
	//UE_LOG(LogTemp, Warning, TEXT("Weekly Exp set in PC: %f"), IdleAttributeSet->GetWeeklyWoodcutExp());
}

void AIdlePlayerState::OnPlayFabPlayerLevelLoaded(int32 LoadedPlayerLevel)
{
	int32 FinalLevel = FMath::Max(LocalPlayerLevel, LoadedPlayerLevel);

	UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(AttributeSet);
	IdleAttributeSet->SetWoodcuttingLevel(LoadedPlayerLevel);
}



