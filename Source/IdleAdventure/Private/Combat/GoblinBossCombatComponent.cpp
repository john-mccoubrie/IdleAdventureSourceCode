


#include "Combat/GoblinBossCombatComponent.h"
#include <PlayerEquipment/BonusManager.h>
#include <Kismet/GameplayStatics.h>
#include <Character/IdleCharacter.h>
#include <Player/IdlePlayerController.h>
#include <Chat/GameChatManager.h>
#include "UI/StoicTypeIndicatorComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include <Combat/CombatManager.h>
#include <AbilitySystem/IdleAttributeSet.h>
#include <Player/IdlePlayerState.h>
#include <Game/SpawnManager.h>
#include <Game/SteamManager.h>

void UGoblinBossCombatComponent::BeginPlay()
{
    Super::BeginPlay();

    CircleSpawnMin = 3.0f;
    CircleSpawnMax = 10.0f;
    CircleHitDamage = 50.0f;
    DamageCircleRadius = 500.0f;
    TimeToDamage = 2.0f;

    // Change Stoic Type every 3 to 10 seconds
    float ChangeInterval = FMath::RandRange(6.0f, 10.0f);
    GetWorld()->GetTimerManager().SetTimer(ChangeStoicTypeTimer, this, &UGoblinBossCombatComponent::ChangeStoicType, ChangeInterval, true);
}

void UGoblinBossCombatComponent::HandleDeath()
{
	Super::HandleDeath();

    ASpawnManager* SpawnManager = ASpawnManager::GetInstance(GetWorld());
    SpawnManager->UpdateBossCount(1);

    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();

    //Update quest
    ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
    AIdleCharacter* Character = Cast<AIdleCharacter>(MyCharacter);
    Character->UpdateAllActiveQuests("BossKills", 1);

    //Unlock Steam Achievement
    ASteamManager* SteamManager = ASteamManager::GetInstance(GetWorld());
    if (SteamManager)
    {
        SteamManager->UnlockSteamAchievement(TEXT("KILL_THE_SHAMAN"));
        SteamManager->UnlockSteamAchievement(TEXT("KILL_A_BOSS"));
    }

    // Set up respawn for tutorial
    AActor* Owner = GetOwner();
    SavedLocation = Owner->GetActorLocation();
    SavedRotation = Owner->GetActorRotation();

    //Handle Exp
    UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);
    IdleAttributeSet->SetWoodcutExp(IdleAttributeSet->GetWoodcutExp() + Experience);
    IdleAttributeSet->SetWeeklyWoodcutExp(IdleAttributeSet->GetWeeklyWoodcutExp() + Experience);
    Character->ShowExpNumber(Experience, Character, FLinearColor::White);

    //Update player controller values
    PC->InteruptCombat();

    // Stop the damage check timer
    if (GetWorld()->GetTimerManager().IsTimerActive(DamageCheckTimer))
    {
        GetWorld()->GetTimerManager().ClearTimer(DamageCheckTimer);
    }

    if (GetWorld()->GetTimerManager().IsTimerActive(ChangeStoicTypeTimer))
    {
        GetWorld()->GetTimerManager().ClearTimer(ChangeStoicTypeTimer);
    }

    //Handle death animation
    AEnemyBase* OwningCharacter = Cast<AEnemyBase>(GetOwner());
    if (OwningCharacter)
    {
        OwningCharacter->EndCombatEffects();
        OwningCharacter->EnemyDeathAnimation();
    }

    if (IsTutorialMap())
    {
        UE_LOG(LogTemp, Warning, TEXT("Countdown started for enemy respawn"));
        //ASpawnManager* SpawnManager = ASpawnManager::GetInstance(GetWorld());
        SpawnManager->ScheduleRespawn("Boss", TutorialGoblinBossBlueprint, SavedLocation, SavedRotation);
        //GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &UNPCCombatComponent::RespawnEnemy, 5.0f, false);
    }

    // Delay the destruction of the owner
    if (GetWorld())
    {
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UGoblinBossCombatComponent::DestroyOwner, 1.0f, false);
    }
}

void UGoblinBossCombatComponent::TakeDamage(float amount, float level)
{
	//Super::TakeDamage(float amount);
	UE_LOG(LogTemp, Warning, TEXT("Take Damage called in GoblinBossCombatComponent"));
	ABonusManager* bonusManager = ABonusManager::GetInstance(GetWorld());

    //InitializeDamagingCircleTimer();

	// Retrieve and apply damage bonuses
	PendingDamage = amount + bonusManager->MultiplierSet.Damage;

	if (!GetWorld()->GetTimerManager().IsTimerActive(DamageCheckTimer))
	{
		// Start a repeating timer that checks for damage every second
		GetWorld()->GetTimerManager().SetTimer(DamageCheckTimer, this, &UGoblinBossCombatComponent::DamageCheck, 1.0f, true);
	}
}

void UGoblinBossCombatComponent::StopDamageCheckTimer()
{
    if (GetWorld()->GetTimerManager().IsTimerActive(DamageCheckTimer))
    {
        GetWorld()->GetTimerManager().ClearTimer(DamageCheckTimer);
    }
}

void UGoblinBossCombatComponent::DamageCheck()
{
    // Getting player's level from the player state
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);
    ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
    float playerLevel = IdleAttributeSet->GetWoodcuttingLevel(); // Replace with actual combat level attribute
    float enemyLevel = this->Level; // Goblin's level

    float baseDamage = PendingDamage; // Base damage

    // Define scaling factors
    float ScalingFactorHigher = 0.05; // Adjust as needed
    float ScalingFactorLower = -0.1; // Adjust as needed

    // Adjust hit probability based on player and enemy levels with distinction
    float levelDifference = enemyLevel - playerLevel; // Note the difference: enemyLevel - playerLevel
    float adjustedLevelDifference = (levelDifference > 0) ? levelDifference * ScalingFactorHigher : levelDifference * ScalingFactorLower;
    float hitProbability = FMath::Clamp(0.5f + adjustedLevelDifference, 0.1f, 0.9f); // Hit probability between 10% and 90%

    FString PlayerWeaponType = GetPlayerWeaponType();

    // Random roll to decide if the hit lands
    if (FMath::RandRange(0.0f, 1.0f) <= hitProbability)
    {
        // Adjusted damage range
        float minDamage = FMath::Max(1.0f, baseDamage * 0.1f); // Minimum damage is at least 1 or 10% of base damage
        float maxDamage = FMath::Max(baseDamage, minDamage); // Ensure max damage isn't lower than min

        // Generate single random damage
        float finalDamage = FMath::RandRange(minDamage, maxDamage);

        // Round to nearest whole number
        int32 finalDamageInt = FMath::RoundToInt(finalDamage);
        finalDamage = static_cast<float>(finalDamageInt);

        if (PlayerWeaponType == CurrentStoicType)
        {
            // Apply damage
            Health -= finalDamage;
            if (OwningCharacter)
            {
                ShowDamageNumber(finalDamage, OwningCharacter, FLinearColor::Red);
            }

            PlayEnemeyHitSound();
        }
        else if (Health <= MaxHealth)
        {
            Health += finalDamage;
            if (OwningCharacter)
            {
                ShowDamageNumber(finalDamage, OwningCharacter, FLinearColor::Green);
            }
        }
        if (Health <= 0)
        {
            HandleDeath();
        }
    }
    else
    {
        if (OwningCharacter)
        {
            ShowDamageNumber(0, OwningCharacter, FLinearColor::White);
        }
    }

    // Broadcast health change
    OnHealthChanged.Broadcast(Health, MaxHealth);
}


FString UGoblinBossCombatComponent::BossChangeType()
{
    int32 RandomNumber = FMath::RandRange(1, 4);

    switch (RandomNumber)
    {
    case 1: return "Wisdom";
    case 2: return "Temperance";
    case 3: return "Justice";
    case 4: return "Courage";
    default: return "Wisdom"; 
    }
}

void UGoblinBossCombatComponent::ChangeStoicType()
{
    CurrentStoicType = BossChangeType();
    ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
    ChangeStoicImage(CurrentStoicType, OwningCharacter);
    //UE_LOG(LogTemp, Warning, TEXT("Current Stoic Type: %s"), *CurrentStoicType);
}

FString UGoblinBossCombatComponent::GetPlayerWeaponType()
{
    ABonusManager* BonusManager = ABonusManager::GetInstance(GetWorld());
    FString WeaponType = BonusManager->MultiplierSet.WeaponTypeIdentifier;
    return WeaponType;
}

void UGoblinBossCombatComponent::InitializeDamagingCircleTimer()
{
    if (!bCanInitializeCircle)
    {
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("Initalize Damage Circle Timer Set"));
    float SpawnInterval = FMath::RandRange(CircleSpawnMin, CircleSpawnMax);
    GetWorld()->GetTimerManager().SetTimer(CircleSpawnTimerHandle, this, &UGoblinBossCombatComponent::SpawnDamagingCircle, SpawnInterval, false);
}

void UGoblinBossCombatComponent::SpawnDamagingCircle()
{
    if (CircleDamageEffect)
    {
        FVector PlayerLocation = UGameplayStatics::GetPlayerCharacter(this, 0)->GetActorLocation();
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CircleDamageEffect, PlayerLocation);

        // Remember the location for damage check
        DamagingCircleCenter = PlayerLocation;
        /*
        // Debug: Draw a sphere representing the damage radius
        DrawDebugSphere(
            GetWorld(),
            DamagingCircleCenter,          // Center of the sphere
            DamageCircleRadius,            // Radius
            32,                            // Segments (quality of the sphere)
            FColor::Red,                   // Color
            false,                         // Persistent (false = single frame)
            5.0f,                          // Duration (seconds)
            0,                             // Depth
            5.0f                           // Thickness
        );
        */

        // Set a timer to check for player in the circle after 3 seconds
        GetWorld()->GetTimerManager().SetTimer(CircleDamageCheckTimerHandle, this, &UGoblinBossCombatComponent::CheckForPlayerInCircle, TimeToDamage, false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("DamagingCircleEffect not set!"));
    }
}

void UGoblinBossCombatComponent::CheckForPlayerInCircle()
{
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ShardsDamageEffect, DamagingCircleCenter);

    AIdleCharacter* PlayerCharacter = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    ACharacter* BossCharacter = Cast<ACharacter>(GetOwner());
    if (PlayerCharacter)
    {
        FVector PlayerLocation = PlayerCharacter->GetActorLocation();
        float DistanceToCenter = FVector::Dist(PlayerLocation, DamagingCircleCenter);

        if (DistanceToCenter <= DamageCircleRadius)
        {
            UE_LOG(LogTemp, Error, TEXT("Player take 50 damage"));
            ACombatManager* CombatManager = ACombatManager::GetInstance(GetWorld());
            CombatManager->HandleCombat(this, PlayerCharacter->CombatComponent, 190);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Player does NOT take 50 damage"));
        }
    }

    // Reset or set a new timer for the next circle
    InitializeDamagingCircleTimer();
    
}

void UGoblinBossCombatComponent::DestroyOwner()
{
    if (AActor* Owner = GetOwner())
    {
        Owner->Destroy();
    }
}

bool UGoblinBossCombatComponent::CanInitializeCircleTimer()
{
    return false;
}

//called from BP_NPCAIController
void UGoblinBossCombatComponent::StopCircleDamageCheckTimer()
{
    Super::StopCircleDamageCheckTimer();
    
    bCanInitializeCircle = false;

    UE_LOG(LogTemp, Warning, TEXT("StopCircleDamageCheckTimer called in goblinbosscombatcomponent"));
}

//called from BP_NPCAIController
void UGoblinBossCombatComponent::StartCircleDamageCheckTimer()
{
    Super::StartCircleDamageCheckTimer();

    bCanInitializeCircle = true;
    InitializeDamagingCircleTimer();

    UE_LOG(LogTemp, Warning, TEXT("StartCircleDamageCheckTimer called in goblinbosscombatcomponent"));
}

void UGoblinBossCombatComponent::RespawnEnemy()
{
}

bool UGoblinBossCombatComponent::IsTutorialMap()
{
    if (GetWorld())
    {
        // Get the current level name
        FString LevelName = GetWorld()->GetMapName();
        //LevelName = UGameplayStatics::RemoveLevelPrefix(LevelName); // Optional: Clean the level name

        // Log the level name for debugging
        UE_LOG(LogTemp, Warning, TEXT("Current level name: %s"), *LevelName);

        // Check if it's the TutorialMap
        return LevelName.Equals("TutorialMap", ESearchCase::IgnoreCase);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GetWorld() returned nullptr, unable to determine level name."));
    }

    return false;
}
