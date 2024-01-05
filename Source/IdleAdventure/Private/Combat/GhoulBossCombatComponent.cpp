


#include "Combat/GhoulBossCombatComponent.h"
#include <Game/SpawnManager.h>
#include <Player/IdlePlayerController.h>
#include <Player/IdlePlayerState.h>
#include <Character/IdleCharacter.h>
#include <Kismet/GameplayStatics.h>
#include <AbilitySystem/IdleAttributeSet.h>
#include <PlayerEquipment/BonusManager.h>
#include <Character/Enemy_Ghoul_Boss.h>
#include <Combat/CombatManager.h>
#include <Chat/GameChatManager.h>
#include <NiagaraFunctionLibrary.h>
#include <Game/SteamManager.h>

void UGhoulBossCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	SwipeAttackIntervalMin = 5.0f;
	SwipeAttackIntervalMax = 15.0f;
    SwipeTimeToDodge = 3.0f;
	SwipeDamageInterval = 2.0f;
	SwipeRange = 1500.0f;
    PoisonDamagePerSecond = 5.0f;
    PoisonDuration = 10.0f;
}

void UGhoulBossCombatComponent::HandleDeath()
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

    //Handle Exp
    UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);
    IdleAttributeSet->SetWoodcutExp(IdleAttributeSet->GetWoodcutExp() + 5000.0f);
    IdleAttributeSet->SetWeeklyWoodcutExp(IdleAttributeSet->GetWeeklyWoodcutExp() + 5000.0f);
    Character->ShowExpNumber(10000.0f, Character, FLinearColor::White);

    //Unlock Steam Achievement
    ASteamManager* SteamManager = ASteamManager::GetInstance(GetWorld());
    if (SteamManager)
    {
        SteamManager->UnlockSteamAchievement(TEXT("KILL_THE_GHOUL"));
        SteamManager->UnlockSteamAchievement(TEXT("KILL_A_BOSS"));
    }

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

void UGhoulBossCombatComponent::TakeDamage(float amount, float level)
{
    //Super::TakeDamage(amount);
    ABonusManager* bonusManager = ABonusManager::GetInstance(GetWorld());

    //InitializeDamagingCircleTimer();

    // Retrieve and apply damage bonuses
    PendingDamage = amount + bonusManager->MultiplierSet.Damage;

    if (!GetWorld()->GetTimerManager().IsTimerActive(DamageCheckTimer))
    {
        // Start a repeating timer that checks for damage every second
        GetWorld()->GetTimerManager().SetTimer(DamageCheckTimer, this, &UGhoulBossCombatComponent::DamageCheck, 1.0f, true);
    }
}

void UGhoulBossCombatComponent::StopDamageCheckTimer()
{
    if (GetWorld()->GetTimerManager().IsTimerActive(DamageCheckTimer))
    {
        GetWorld()->GetTimerManager().ClearTimer(DamageCheckTimer);
    }
}

void UGhoulBossCombatComponent::InitiateSwipeAttack()
{
    // Assuming GetOwner() returns the AEnemy_Demon_Boss instance
    AEnemy_Ghoul_Boss* GhoulBoss = Cast<AEnemy_Ghoul_Boss>(GetOwner());
    if (GhoulBoss && GhoulBoss->GhoulSwipeMontage)
    {
        // Play swipe animation montage
        GhoulBoss->PlayAnimMontage(GhoulBoss->GhoulSwipeMontage);

        // Spawn Niagara effects for poison
        FVector BossLocation = GetOwner()->GetActorLocation();
        BossLocation.Z -= zAdjustmentPoisonSpawn;
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PoisonCircleEffect, BossLocation);

        //Spawn wind up effect
        GhoulBoss->SpawnWindUpEffect();

        // Check if player is in range and apply damage
        GetWorld()->GetTimerManager().SetTimer(SwipeDamageTimer, this, &UGhoulBossCombatComponent::CheckForPlayerInRange, SwipeTimeToDodge, false);
        UE_LOG(LogTemp, Error, TEXT("Need to dodge"));
    }

    bIsSwipeAttackActive = true;
}

void UGhoulBossCombatComponent::CheckForPlayerInRange()
{
    // Check if player is within TailspinRange
    AIdleCharacter* PlayerCharacter = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    //Deactivate wind up effect
    AEnemy_Ghoul_Boss* GhoulBoss = Cast<AEnemy_Ghoul_Boss>(GetOwner());
    GhoulBoss->EndWindUpEffect();

    if (PlayerCharacter)
    {
        FVector PlayerLocation = PlayerCharacter->GetActorLocation();
        FVector BossLocation = GetOwner()->GetActorLocation();
        //BossLocation.Z -= zAdjustmentPoisonSpawn;
        //UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PoisonCircleEffect, BossLocation);
        float DistanceToPlayer = FVector::Dist(PlayerLocation, BossLocation);

        if (DistanceToPlayer <= SwipeRange)
        {
            UE_LOG(LogTemp, Error, TEXT("Player takes damage"));

            ApplyPoisonEffectToPlayer(PlayerCharacter);

            AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
            GameChatManager->PostNotificationToUI(TEXT("You have been poisoned!"), FLinearColor::Green);

            //return to normal attack sequence
            StopSwipeAttack();

            // Player is in range, start applying damage at intervals
            GetWorld()->GetTimerManager().SetTimer(SwipeDamageTimer, this, &UGhoulBossCombatComponent::ApplySwipeDamage, SwipeDamageInterval, true);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Distance to player: %f"), DistanceToPlayer);
            UE_LOG(LogTemp, Error, TEXT("Swipe range: %f"), SwipeRange);
            UE_LOG(LogTemp, Error, TEXT("Out of range"));
            StopSwipeAttack();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Player character is not valid"));
    }
}

void UGhoulBossCombatComponent::ApplyPoisonEffectToPlayer(AIdleCharacter* PlayerCharacter)
{
    if (PlayerCharacter)
    {
        // Reset the poison duration
        PoisonDuration = 10.0f;

        // Start a timer to apply poison damage over time
        GetWorld()->GetTimerManager().SetTimer(
            PoisonTimerHandle,
            this,
            &UGhoulBossCombatComponent::DealPoisonDamage,
            1.0f, // Every second
            true
        );
    }
    FVector PlayerLocation = PlayerCharacter->GetActorLocation();
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PoisonDamageEffect, PlayerLocation);
}

void UGhoulBossCombatComponent::DealPoisonDamage()
{
    AIdleCharacter* PlayerCharacter = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (PlayerCharacter)
    {
        // Deal poison damage
        ACombatManager* CombatManager = ACombatManager::GetInstance(GetWorld());
        CombatManager->HandleCombat(this, PlayerCharacter->CombatComponent, PoisonDamagePerSecond);

        // Show damage number
        if (PlayerCharacter)
        {
            //ShowDamageNumber(PoisonDamagePerSecond, PlayerCharacter, FLinearColor::Yellow);
        }

        // Reduce poison duration
        PoisonDuration -= 1.0f;
        if (PoisonDuration <= 0)
        {
            StopPoisonEffect();
        }
    }
}

void UGhoulBossCombatComponent::StopPoisonEffect()
{
    // Clear the poison timer if active
    if (GetWorld()->GetTimerManager().IsTimerActive(PoisonTimerHandle))
    {
        GetWorld()->GetTimerManager().ClearTimer(PoisonTimerHandle);
    }
}

void UGhoulBossCombatComponent::ApplySwipeDamage()
{
    // Apply damage to the player
    AIdleCharacter* PlayerCharacter = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    FVector PlayerLocation = PlayerCharacter->GetActorLocation();
    FVector BossLocation = GetOwner()->GetActorLocation();
    float DistanceToPlayer = FVector::Dist(PlayerLocation, BossLocation);
    if (DistanceToPlayer <= SwipeRange)
    {
        ACombatManager* CombatManager = ACombatManager::GetInstance(GetWorld());
        CombatManager->HandleCombat(this, PlayerCharacter->CombatComponent, 150);
    }
    else
    {
        StopSwipeAttack();
        StopSwipeDamage();
    }
}

void UGhoulBossCombatComponent::StopSwipeDamage()
{
    if (GetWorld()->GetTimerManager().IsTimerActive(SwipeDamageTimer))
    {
        GetWorld()->GetTimerManager().ClearTimer(SwipeDamageTimer);
    }
}

void UGhoulBossCombatComponent::StopSwipeAttack()
{
    // Reset the tailspin attack timer with a new random interval
    GetWorld()->GetTimerManager().ClearTimer(SwipeAttackTimer);
    float NewAttackInterval = FMath::RandRange(SwipeAttackIntervalMin, SwipeAttackIntervalMax);
    GetWorld()->GetTimerManager().SetTimer(SwipeAttackTimer, this, &UGhoulBossCombatComponent::InitiateSwipeAttack, NewAttackInterval, false);

    if (GetWorld()->GetTimerManager().IsTimerActive(SwipeDamageTimer))
    {
        GetWorld()->GetTimerManager().ClearTimer(SwipeDamageTimer);
    }

    AEnemy_Ghoul_Boss* GhoulBoss = Cast<AEnemy_Ghoul_Boss>(GetOwner());
    if (GhoulBoss && GhoulBoss->GhoulSwipeMontage)
    {
        GhoulBoss->StopAnimMontage(GhoulBoss->GhoulSwipeMontage);
    }

    // Reset tailspin active flag
    bIsSwipeAttackActive = false;
}

void UGhoulBossCombatComponent::InitializeSwipeAttackTimer()
{
    // Start the swipe attack timer
    UE_LOG(LogTemp, Warning, TEXT("Initialize tailspin attack timer"));
    float AttackInterval = FMath::RandRange(SwipeAttackIntervalMin, SwipeAttackIntervalMax);
    GetWorld()->GetTimerManager().SetTimer(SwipeAttackTimer, this, &UGhoulBossCombatComponent::InitiateSwipeAttack, AttackInterval, true);
}

void UGhoulBossCombatComponent::DamageCheck()
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
