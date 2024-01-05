


#include "Combat/DemonBossCombatComponent.h"
#include <Player/IdlePlayerController.h>
#include <Player/IdlePlayerState.h>
#include <Character/IdleCharacter.h>
#include <AbilitySystem/IdleAttributeSet.h>
#include <PlayerEquipment/BonusManager.h>
#include <Kismet/GameplayStatics.h>
#include <Game/SpawnManager.h>
#include <Combat/CombatManager.h>
#include <Character/Enemy_Demon_Boss.h>
#include <Game/SteamManager.h>

void UDemonBossCombatComponent::BeginPlay()
{
    Super::BeginPlay();

    // Initialize Tailspin attack parameters
    TailspinAttackIntervalMin = 5.0f; // Minimum time between attacks
    TailspinAttackIntervalMax = 12.0f; // Maximum time between attacks
    TailspinDamageInterval = 0.25f; // Time between damage applications
    TailspinRange = 1000.0f; // Range within which the player takes damage
}

void UDemonBossCombatComponent::HandleDeath()
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
        SteamManager->UnlockSteamAchievement(TEXT("KILL_THE_DEMON"));
        SteamManager->UnlockSteamAchievement(TEXT("KILL_A_BOSS"));
    }

    //Handle Exp
    UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);
    IdleAttributeSet->SetWoodcutExp(IdleAttributeSet->GetWoodcutExp() + 5000.0f);
    IdleAttributeSet->SetWeeklyWoodcutExp(IdleAttributeSet->GetWeeklyWoodcutExp() + 5000.0f);
    Character->ShowExpNumber(5000.0f, Character, FLinearColor::White);

    //Update player controller values
    PC->InteruptCombat();

    // Stop the damage check timer
    if (GetWorld()->GetTimerManager().IsTimerActive(DamageCheckTimer))
    {
        GetWorld()->GetTimerManager().ClearTimer(DamageCheckTimer);
    }

    //Destroy the enemy actor
    if (AActor* Owner = GetOwner())
    {
        Owner->Destroy();
    }
}

void UDemonBossCombatComponent::TakeDamage(float amount, float level)
{
    //Super::TakeDamage(amount);
	ABonusManager* bonusManager = ABonusManager::GetInstance(GetWorld());

	//InitializeDamagingCircleTimer();

	// Retrieve and apply damage bonuses
	PendingDamage = amount + bonusManager->MultiplierSet.Damage;

	if (!GetWorld()->GetTimerManager().IsTimerActive(DamageCheckTimer))
	{
		// Start a repeating timer that checks for damage every second
		GetWorld()->GetTimerManager().SetTimer(DamageCheckTimer, this, &UDemonBossCombatComponent::DamageCheck, 1.0f, true);
	}
}

void UDemonBossCombatComponent::StopDamageCheckTimer()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(DamageCheckTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(DamageCheckTimer);
	}
}

void UDemonBossCombatComponent::InitiateTailspinAttack()
{
    // Assuming GetOwner() returns the AEnemy_Demon_Boss instance
    AEnemy_Demon_Boss* DemonBoss = Cast<AEnemy_Demon_Boss>(GetOwner());
    if (DemonBoss && DemonBoss->DemonTailspinMontage)
    {
        // Play tailspin animation montage
        DemonBoss->PlayAnimMontage(DemonBoss->DemonTailspinMontage);

        // Check if player is in range and apply damage
        CheckForPlayerInRange();

        // Optionally, move towards the player
        MoveTowardsPlayer();
    }

    bIsTailspinActive = true;
}

void UDemonBossCombatComponent::CheckForPlayerInRange()
{
    // Check if player is within TailspinRange
    AIdleCharacter* PlayerCharacter = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (PlayerCharacter)
    {
        FVector PlayerLocation = PlayerCharacter->GetActorLocation();
        FVector BossLocation = GetOwner()->GetActorLocation();
        float DistanceToPlayer = FVector::Dist(PlayerLocation, BossLocation);

        if (DistanceToPlayer <= TailspinRange)
        {
            UE_LOG(LogTemp, Error, TEXT("Player takes 50 damage"));
            //ACombatManager* CombatManager = ACombatManager::GetInstance(GetWorld());
            //CombatManager->HandleCombat(this, PlayerCharacter->CombatComponent, 50);
            // Player is in range, start applying damage at intervals
            GetWorld()->GetTimerManager().SetTimer(TailspinDamageTimer, this, &UDemonBossCombatComponent::ApplyTailspinDamage, TailspinDamageInterval, true);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Distance to player: %f"), DistanceToPlayer);
            UE_LOG(LogTemp, Error, TEXT("Tailspin range: %f"), TailspinRange);
            UE_LOG(LogTemp, Error, TEXT("Out of range"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Player character is not valid"));
    }
}

void UDemonBossCombatComponent::MoveTowardsPlayer()
{
    // Move the boss towards the player slowly
    // Implement movement logic here...
}

void UDemonBossCombatComponent::ApplyTailspinDamage()
{
    // Apply damage to the player
    AIdleCharacter* PlayerCharacter = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    FVector PlayerLocation = PlayerCharacter->GetActorLocation();
    FVector BossLocation = GetOwner()->GetActorLocation();
    float DistanceToPlayer = FVector::Dist(PlayerLocation, BossLocation);
    if (DistanceToPlayer <= TailspinRange)
    {
        ACombatManager* CombatManager = ACombatManager::GetInstance(GetWorld());
        CombatManager->HandleCombat(this, PlayerCharacter->CombatComponent, 10);
    }
    else
    {
        StopTailspinAttack();
        StopTailspinDamage();
    } 
}

void UDemonBossCombatComponent::StopTailspinDamage()
{
    if (GetWorld()->GetTimerManager().IsTimerActive(TailspinDamageTimer))
    {
        GetWorld()->GetTimerManager().ClearTimer(TailspinDamageTimer);
    }
}

void UDemonBossCombatComponent::StopTailspinAttack()
{
    // Reset the tailspin attack timer with a new random interval
    GetWorld()->GetTimerManager().ClearTimer(TailspinAttackTimer);
    float NewAttackInterval = FMath::RandRange(TailspinAttackIntervalMin, TailspinAttackIntervalMax);
    GetWorld()->GetTimerManager().SetTimer(TailspinAttackTimer, this, &UDemonBossCombatComponent::InitiateTailspinAttack, NewAttackInterval, false);

    if (GetWorld()->GetTimerManager().IsTimerActive(TailspinDamageTimer))
    {
        GetWorld()->GetTimerManager().ClearTimer(TailspinDamageTimer);
    }

    AEnemy_Demon_Boss* DemonBoss = Cast<AEnemy_Demon_Boss>(GetOwner());
    if (DemonBoss && DemonBoss->DemonTailspinMontage)
    {
        DemonBoss->StopAnimMontage(DemonBoss->DemonTailspinMontage);
    }

    // Reset tailspin active flag
    bIsTailspinActive = false;
}

void UDemonBossCombatComponent::InitializeTailspinAttackTimer()
{
    // Start the tailspin attack timer
    UE_LOG(LogTemp, Warning, TEXT("Initialize tailspin attack timer"));
    float AttackInterval = FMath::RandRange(TailspinAttackIntervalMin, TailspinAttackIntervalMax);
    GetWorld()->GetTimerManager().SetTimer(TailspinAttackTimer, this, &UDemonBossCombatComponent::InitiateTailspinAttack, AttackInterval, true);
}

void UDemonBossCombatComponent::DamageCheck()
{
    // Getting player's level from the player state
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);
    float playerLevel = IdleAttributeSet->GetWoodcuttingLevel(); // Replace with actual combat level attribute
    float goblinLevel = this->Level; // Goblin's level

    float baseDamage = PendingDamage; // Base damage

    // Calculate hit probability based on player and goblin levels
    float levelRatio = playerLevel / goblinLevel;
    float hitProbability = FMath::Clamp(levelRatio * 0.5f, 0.0f, 1.0f); // Adjust for higher miss chance

    // Random roll to decide if the hit lands
    if (FMath::RandRange(0.0f, 1.0f) <= hitProbability)
    {
        // Skew the damage range towards lower values
        float minDamage = 2.0f; // Fixed minimum
        float maxDamage = baseDamage; // Max damage remains as base damage

        // Generate two random numbers and use the lower one for more frequent low damage hits
        float randomDamage1 = FMath::RandRange(minDamage, maxDamage);
        float randomDamage2 = FMath::RandRange(minDamage, maxDamage);
        float finalDamage = FMath::Min(randomDamage1, randomDamage2);

        // Round to nearest whole number
        int32 finalDamageInt = FMath::RoundToInt(finalDamage);
        finalDamage = static_cast<float>(finalDamageInt);

        // Apply damage
        Health -= finalDamage;
        if (Health <= 0)
        {
            HandleDeath();
        }

        // Show damage number (red for damage, white for miss)
        ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
        if (OwningCharacter)
        {
            ShowDamageNumber(finalDamage, OwningCharacter, FLinearColor::Red);
        }
    }
    else
    {
        // Missed attack
        ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
        if (OwningCharacter)
        {
            ShowDamageNumber(0, OwningCharacter, FLinearColor::White);
        }
    }

    // Broadcast health change
    OnHealthChanged.Broadcast(Health, MaxHealth);
}
