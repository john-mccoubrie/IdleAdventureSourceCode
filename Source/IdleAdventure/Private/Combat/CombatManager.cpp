


#include "Combat/CombatManager.h"
#include "EngineUtils.h"
#include <PlayerEquipment/BonusManager.h>

ACombatManager* ACombatManager::CombatManagerSingletonInstance = nullptr;

// Sets default values
ACombatManager::ACombatManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACombatManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (!CombatManagerSingletonInstance)
	{
		CombatManagerSingletonInstance = this;
	}
}

void ACombatManager::BeginDestroy()
{
	Super::BeginDestroy();
	ResetInstance();
}

ACombatManager* ACombatManager::GetInstance(UWorld* World)
{
    if (!CombatManagerSingletonInstance)
    {
        for (TActorIterator<ACombatManager> It(World); It; ++It)
        {
            CombatManagerSingletonInstance = *It;
            break;
        }
        if (!CombatManagerSingletonInstance)
        {
            CombatManagerSingletonInstance = World->SpawnActor<ACombatManager>();
        }
    }
    return CombatManagerSingletonInstance;
}

void ACombatManager::ResetInstance()
{
    CombatManagerSingletonInstance = nullptr;
}

void ACombatManager::HandleCombat(UBaseCombatComponent* attacker, UBaseCombatComponent* defender, int32 DamageMultiplier)
{
    //UE_LOG(LogTemp, Warning, TEXT("Handle combat in combat manager"));
    //check range, attack type, etc.
    if (defender)
    {
        if (attacker)
        {
            // Apply the calculated damage
            defender->TakeDamage(attacker->Damage + DamageMultiplier);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("attacker is null in combat component"));
        }
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("defender is null in combat component"));
    }
    
}

void ACombatManager::HandleMultiTargetCombat(UBaseCombatComponent* Attacker, const TArray<UBaseCombatComponent*>& Defenders)
{
    for (UBaseCombatComponent* Defender : Defenders)
    {
        // Similar logic to HandleCombat
        // Apply damage to each defender
    }
}


