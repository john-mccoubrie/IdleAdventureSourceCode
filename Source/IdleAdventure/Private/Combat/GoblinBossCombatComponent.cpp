


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

void UGoblinBossCombatComponent::BeginPlay()
{
    Super::BeginPlay();

    CircleSpawnMin = 3.0f;
    CircleSpawnMax = 10.0f;
    CircleHitDamage = 50.0f;
    DamageCircleRadius = 500.0f;
    TimeToDamage = 2.0f;

    // Change Stoic Type every 3 to 10 seconds
    float ChangeInterval = FMath::RandRange(3.0f, 10.0f);
    GetWorld()->GetTimerManager().SetTimer(ChangeStoicTypeTimer, this, &UGoblinBossCombatComponent::ChangeStoicType, ChangeInterval, true);
}

void UGoblinBossCombatComponent::HandleDeath()
{
	Super::HandleDeath();

    //Update quest
    ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
    AIdleCharacter* Character = Cast<AIdleCharacter>(MyCharacter);
    Character->UpdateAllActiveQuests("BossKills", 1);

    //Handle Exp

    //Update player controller values
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
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

	//Destroy the enemy actor
	if (AActor* Owner = GetOwner())
	{
		Owner->Destroy();
	}
}

void UGoblinBossCombatComponent::TakeDamage(float amount)
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
    ABonusManager* BonusManager = ABonusManager::GetInstance(GetWorld());

    FString PlayerWeaponType = GetPlayerWeaponType();
        if (PlayerWeaponType == CurrentStoicType)
        {
            Health -= PendingDamage;
            if (Health <= 0)
            {
                HandleDeath();
            }

            ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
            if (OwningCharacter)
            {
                ShowDamageNumber(PendingDamage, OwningCharacter, FLinearColor::Red);
            }

            OnHealthChanged.Broadcast(Health, MaxHealth);
        }
        else if(Health <= MaxHealth)
        {
            Health += PendingDamage;
            
            AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
            //GameChatManager->PostNotificationToUI(TEXT("OH NO! Your staff is healing the Goblin!!!"), FLinearColor::Yellow);

            ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
            if (OwningCharacter)
            {
                ShowDamageNumber(PendingDamage, OwningCharacter, FLinearColor::Green);
            }

            OnHealthChanged.Broadcast(Health, MaxHealth);
        }
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
            CombatManager->HandleCombat(this, PlayerCharacter->CombatComponent);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Player does NOT take 50 damage"));
        }
    }

    // Reset or set a new timer for the next circle
    InitializeDamagingCircleTimer();
}
