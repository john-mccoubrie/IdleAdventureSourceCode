


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
    //base experience bonus
}

void ABonusManager::ApplyEssenceBonus(const FItemBonus& ItemBonus)
{
    ActiveBonuses.Add(ItemBonus); // Add the new bonus to the list

    // Accumulate the bonuses
    WisdomEssenceMultiplier *= ItemBonus.WisdomEssenceMultiplier;
    TemperanceEssenceMultiplier *= ItemBonus.TemperanceEssenceMultiplier;
    JusticeEssenceMultiplier *= ItemBonus.JusticeEssenceMultiplier;
    CourageEssenceMultiplier *= ItemBonus.CourageWisdomEssenceMultiplier;
    LegendaryEssenceMultiplier *= ItemBonus.LegendaryEssenceMultiplier;

    WisdomYieldMultiplier += (ItemBonus.WisdomYieldMultiplier);
    TemperanceYieldMultiplier += (ItemBonus.TemperanceYieldMultiplier);
    JusticeYieldMultiplier += (ItemBonus.JusticeYieldMultiplier);
    CourageYieldMultiplier += (ItemBonus.CourageYieldMultiplier);
    LegendaryYieldMultiplier += (ItemBonus.LegendaryYieldMultiplier);

    OnBonusesUpdated.Broadcast(WisdomEssenceMultiplier, TemperanceEssenceMultiplier, 
        JusticeEssenceMultiplier, CourageEssenceMultiplier, WisdomYieldMultiplier, TemperanceYieldMultiplier, JusticeYieldMultiplier, CourageYieldMultiplier);

    /*
    UE_LOG(LogTemp, Warning, TEXT("===== Applying Essence Bonuses ====="));
    UE_LOG(LogTemp, Warning, TEXT("Item Bonuses:"));
    UE_LOG(LogTemp, Warning, TEXT("Wisdom Essence Bonus: %f"), ItemBonus.WisdomEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Temperance Essence Bonus: %f"), ItemBonus.TemperanceEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Justice Essence Bonus: %f"), ItemBonus.JusticeEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Courage Essence Bonus: %f"), ItemBonus.CourageWisdomEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("===== Applying Yield Bonuses ====="));
    UE_LOG(LogTemp, Warning, TEXT("Item Yield Bonuses Being Applied:"));
    UE_LOG(LogTemp, Warning, TEXT("Wisdom Yield Bonus: %i"), ItemBonus.WisdomYieldMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Temperance Yield Bonus: %i"), ItemBonus.TemperanceYieldMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Justice Yield Bonus: %i"), ItemBonus.JusticeYieldMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Courage Yield Bonus: %i"), ItemBonus.CourageYieldMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Legendary Yield Bonus: %i"), ItemBonus.LegendaryYieldMultiplier);
    */
    UE_LOG(LogTemp, Warning, TEXT("Current Multipliers After Applying Bonuses:"));

    UE_LOG(LogTemp, Warning, TEXT("Applied Essence Bonuses:"));
    UE_LOG(LogTemp, Warning, TEXT("Wisdom Essence Multiplier: %f"), WisdomEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Temperance Essence Multiplier: %f"), TemperanceEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Justice Essence Multiplier: %f"), JusticeEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Courage Essence Multiplier: %f"), CourageEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Legendary Essence Multiplier: %f"), LegendaryEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Wisdom Yield Multiplier: %i"), WisdomYieldMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Temperance Yield Multiplier: %i"), TemperanceYieldMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Justice Yield Multiplier: %i"), JusticeYieldMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Courage Yield Multiplier: %i"), CourageYieldMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Legendary Yield Multiplier: %i"), LegendaryYieldMultiplier);
}

void ABonusManager::RemoveEssenceBonus(const FItemBonus& ItemBonus)
{
    ActiveBonuses.Remove(ItemBonus); // Remove the bonus from the list

    // Subtract the bonuses
    WisdomEssenceMultiplier /= ItemBonus.WisdomEssenceMultiplier;
    TemperanceEssenceMultiplier /= ItemBonus.TemperanceEssenceMultiplier;
    JusticeEssenceMultiplier /= ItemBonus.JusticeEssenceMultiplier;
    CourageEssenceMultiplier /= ItemBonus.CourageWisdomEssenceMultiplier;
    LegendaryEssenceMultiplier /= ItemBonus.LegendaryEssenceMultiplier;

    WisdomYieldMultiplier -= (ItemBonus.WisdomYieldMultiplier);
    TemperanceYieldMultiplier -= (ItemBonus.TemperanceYieldMultiplier);
    JusticeYieldMultiplier -= (ItemBonus.JusticeYieldMultiplier);
    CourageYieldMultiplier -= (ItemBonus.CourageYieldMultiplier);
    LegendaryYieldMultiplier -= (ItemBonus.LegendaryYieldMultiplier);

    OnBonusesUpdated.Broadcast(WisdomEssenceMultiplier, TemperanceEssenceMultiplier, 
        JusticeEssenceMultiplier, CourageEssenceMultiplier, WisdomYieldMultiplier, TemperanceYieldMultiplier, JusticeYieldMultiplier, CourageYieldMultiplier);

    /*
    UE_LOG(LogTemp, Warning, TEXT("===== Removing Essence Bonuses ====="));
    UE_LOG(LogTemp, Warning, TEXT("Item Bonuses Being Removed:"));
    UE_LOG(LogTemp, Warning, TEXT("Wisdom Essence Bonus: %f"), ItemBonus.WisdomEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Temperance Essence Bonus: %f"), ItemBonus.TemperanceEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Justice Essence Bonus: %f"), ItemBonus.JusticeEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Courage Essence Bonus: %f"), ItemBonus.CourageWisdomEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("===== Removing Yield Bonuses ====="));
    UE_LOG(LogTemp, Warning, TEXT("Item Yield Bonuses Being Removed:"));
    UE_LOG(LogTemp, Warning, TEXT("Wisdom Yield Bonus: %i"), ItemBonus.WisdomYieldMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Temperance Yield Bonus: %i"), ItemBonus.TemperanceYieldMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Justice Yield Bonus: %i"), ItemBonus.JusticeYieldMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Courage Yield Bonus: %i"), ItemBonus.CourageYieldMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Legendary Yield Bonus: %i"), ItemBonus.LegendaryYieldMultiplier);
    */
    UE_LOG(LogTemp, Warning, TEXT("===== Current Multipliers After Removing Bonuses =====:"));
    UE_LOG(LogTemp, Warning, TEXT("Wisdom Essence Multiplier: %f"), WisdomEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Temperance Essence Multiplier: %f"), TemperanceEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Justice Essence Multiplier: %f"), JusticeEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Courage Essence Multiplier: %f"), CourageEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Legendary Essence Multiplier: %f"), LegendaryEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Wisdom Yield Multiplier: %i"), WisdomYieldMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Temperance Yield Multiplier: %i"), TemperanceYieldMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Justice Yield Multiplier: %i"), JusticeYieldMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Courage Yield Multiplier: %i"), CourageYieldMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Legendary Yield Multiplier: %i"), LegendaryYieldMultiplier);
}

