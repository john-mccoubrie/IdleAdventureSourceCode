


#include "PlayerEquipment/BonusManager.h"
#include "EngineUtils.h"

ABonusManager* ABonusManager::BonusManagerSingletonInstance = nullptr;

ABonusManager::ABonusManager()
{

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
    WisdomEssenceMultiplier = ItemBonus.WisdomEssenceMultiplier;
    TemperanceEssenceMultiplier = ItemBonus.TemperanceEssenceMultiplier;
    JusticeEssenceMultiplier = ItemBonus.JusticeEssenceMultiplier;
    CourageEssenceMultiplier = ItemBonus.CourageWisdomEssenceMultiplier;

    UE_LOG(LogTemp, Warning, TEXT("Applied Essence Bonuses:"));
    UE_LOG(LogTemp, Warning, TEXT("Wisdom Essence Multiplier: %f"), WisdomEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Temperance Essence Multiplier: %f"), TemperanceEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Justice Essence Multiplier: %f"), JusticeEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Courage Essence Multiplier: %f"), CourageEssenceMultiplier);
}

void ABonusManager::RemoveEssenceBonus()
{
    WisdomEssenceMultiplier = 1.0f;
    TemperanceEssenceMultiplier = 1.0f;
    JusticeEssenceMultiplier = 1.0f;
    CourageEssenceMultiplier = 1.0f;

    UE_LOG(LogTemp, Warning, TEXT("REMOVING Applied Essence Bonuses:"));
    UE_LOG(LogTemp, Warning, TEXT("Wisdom Essence Multiplier: %f"), WisdomEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Temperance Essence Multiplier: %f"), TemperanceEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Justice Essence Multiplier: %f"), JusticeEssenceMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("Courage Essence Multiplier: %f"), CourageEssenceMultiplier);
}

