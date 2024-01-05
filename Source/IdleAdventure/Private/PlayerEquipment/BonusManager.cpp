


#include "PlayerEquipment/BonusManager.h"
#include "EngineUtils.h"

ABonusManager* ABonusManager::BonusManagerSingletonInstance = nullptr;

ABonusManager::ABonusManager()
{
    WisdomEssenceMultiplier = 1.0f;
    TemperanceEssenceMultiplier = 1.0f;
    JusticeEssenceMultiplier = 1.0f;
    CourageEssenceMultiplier = 1.0f;
    LegendaryEssenceMultiplier = 1.0f;
    WisdomYieldMultiplier = 0;
    TemperanceYieldMultiplier = 0;
    JusticeYieldMultiplier = 0;
    CourageYieldMultiplier = 0;
    LegendaryYieldMultiplier = 0;

    WisdomEssenceChanceMultiplier = 0.0f;
    TemperanceEssenceChanceMultiplier = 0.0f;
    JusticeEssenceChanceMultiplier = 0.0f;
    CourageEssenceChanceMultiplier = 0.0f;

    InvestingBonusChance = 0;

    Damage = 0.0f;
    Armor = 0.0f;

    Speed = 1000.0f;

    WeaponTypeIdentifier = "None";
}

void ABonusManager::BeginPlay()
{
    Super::BeginPlay();

	if (!BonusManagerSingletonInstance)
	{
		BonusManagerSingletonInstance = this;
	}
}

void ABonusManager::BeginDestroy()
{
	Super::BeginDestroy();

	ResetInstance();
}

ABonusManager* ABonusManager::GetInstance(UWorld* World)
{
    if (!BonusManagerSingletonInstance)
    {
        for (TActorIterator<ABonusManager> It(World); It; ++It)
        {
            BonusManagerSingletonInstance = *It;
            break;
        }
        if (!BonusManagerSingletonInstance)
        {
            BonusManagerSingletonInstance = World->SpawnActor<ABonusManager>();
        }
    }
    return BonusManagerSingletonInstance;
}

void ABonusManager::ResetInstance()
{
    BonusManagerSingletonInstance = nullptr;
}

void ABonusManager::ApplyExperienceBonus(float Multiplier)
{
    
}

void ABonusManager::ApplyEssenceBonus(const FItemBonus& ItemBonus)
{
    ActiveBonuses.Add(ItemBonus);

    MultiplierSet.ApplyBonus(ItemBonus);
    OnBonusesUpdated.Broadcast(MultiplierSet);
}

void ABonusManager::RemoveEssenceBonus(const FItemBonus& ItemBonus)
{
    ActiveBonuses.Remove(ItemBonus);

    MultiplierSet.RemoveBonus(ItemBonus);
    OnBonusesUpdated.Broadcast(MultiplierSet);
}

void ABonusManager::UpdateStaffName(FString StaffName)
{
    OnBroadcastStaff.Broadcast(StaffName);
}

void ABonusManager::UpdateCapeName(FString CapeName)
{
    OnBroadcastCape.Broadcast(CapeName);
}

void ABonusManager::UpdateAuraName(FString AuraName)
{
    OnBroadcastAura.Broadcast(AuraName);
}


