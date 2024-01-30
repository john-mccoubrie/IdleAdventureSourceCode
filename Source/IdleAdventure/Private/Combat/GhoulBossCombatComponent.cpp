


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

	SwipeAttackIntervalMin = 8.0f;
	SwipeAttackIntervalMax = 15.0f;
    SwipeTimeToDodge = 3.0f;
	SwipeDamageInterval = 2.0f;
	SwipeRange = 4000.0f;
    zAdjustmentPoisonSpawn = -100.0f;
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
    IdleAttributeSet->SetWoodcutExp(IdleAttributeSet->GetWoodcutExp() + Experience);
    IdleAttributeSet->SetWeeklyWoodcutExp(IdleAttributeSet->GetWeeklyWoodcutExp() + Experience);
    Character->ShowExpNumber(Experience, Character, FLinearColor::White);

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

void UGhoulBossCombatComponent::DamageCheck()
{
    Super::DamageCheck();
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
    if (!bCanInitializeCircle)
    {
        return;
    }
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


        // Debug: Draw a sphere representing the damage radius
        DrawDebugSphere(
            GetWorld(),
            BossLocation,          // Center of the sphere
            SwipeRange,            // Radius
            32,                            // Segments (quality of the sphere)
            FColor::Red,                   // Color
            false,                         // Persistent (false = single frame)
            5.0f,                          // Duration (seconds)
            0,                             // Depth
            5.0f                           // Thickness
        );
        


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
            GameChatManager->PostNotificationToUI(TEXT("You have been poisoned! Maybe you need some more speed..."), FLinearColor::Green);

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

void UGhoulBossCombatComponent::StopCircleDamageCheckTimer()
{
    bCanInitializeCircle = false;
    if (SwipeAttackTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(SwipeAttackTimer);
        SwipeAttackTimer.Invalidate();
        UE_LOG(LogTemp, Warning, TEXT("SwipeAttack timer cleared and invalidated"));
    }
    if (SwipeDamageTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(SwipeDamageTimer);
        SwipeDamageTimer.Invalidate();
        UE_LOG(LogTemp, Warning, TEXT("SwipeDamage timer cleared and invalidated"));
    }
    if (PoisonTimerHandle.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(PoisonTimerHandle);
        PoisonTimerHandle.Invalidate();
        UE_LOG(LogTemp, Warning, TEXT("PoisonTimerHandle timer cleared and invalidated"));
    }
    
}

void UGhoulBossCombatComponent::StartCircleDamageCheckTimer()
{
    Super::StartCircleDamageCheckTimer();

    bCanInitializeCircle = true;
    StopSwipeAttack();

    UE_LOG(LogTemp, Warning, TEXT("StartSwipeAttackTimer called in ghoulbosscombatcomponent"));
}

