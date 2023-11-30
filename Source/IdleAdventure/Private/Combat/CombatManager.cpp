


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

void ACombatManager::HandleCombat(UBaseCombatComponent* attacker, UBaseCombatComponent* defender)
{
    //check range, attack type, etc.
    ABonusManager* bonusManager = ABonusManager::GetInstance(GetWorld());

    // Retrieve and apply damage bonuses
    float DamageBonus = bonusManager->MultiplierSet.Damage;
    float TotalDamage = attacker->Damage + DamageBonus;

    UE_LOG(LogTemp, Warning, TEXT("Total Damage %f"), TotalDamage);

    // Apply the calculated damage
    defender->TakeDamage(TotalDamage);
}


