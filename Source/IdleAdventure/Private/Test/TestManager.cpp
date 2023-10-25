
#include "Test/TestManager.h"
#include "EngineUtils.h"

ATestManager* ATestManager::TestManagerSingletonInstance = nullptr;

// Sets default values
ATestManager::ATestManager()
{
    // Set default values for the game settings
    CurrentSettings.EssenceYieldSpeed = 1.0f;
    CurrentSettings.LevelUpMultiplier = 1.50f; 
    CurrentSettings.CofferMeterReductionRate = 1.0f;
    CurrentSettings.EquipmentBonusAmout = 1.0f; 
    CurrentSettings.EssenceYieldAmount = 1.0f; 
    CurrentSettings.LegendaryExpAmount = 1.0f; 
    CurrentSettings.TreeCutTimeMin = 10.0f;
    CurrentSettings.TreeCutTimeMax = 15.0f;
    CurrentSettings.TreeRespawnDelay = 5.0f;
    CurrentSettings.CofferDropOffTime = 10.0f;
    UE_LOG(LogTemp, Warning, TEXT("Default values set"));

}

void ATestManager::ChangeSettings(FGameSettings NewSettings)
{
	CurrentSettings = NewSettings;
	OnSettingsChanged.Broadcast(CurrentSettings);

    UE_LOG(LogTemp, Warning, TEXT("EssenceYieldSpeed: %f"), CurrentSettings.EssenceYieldSpeed);
    UE_LOG(LogTemp, Warning, TEXT("LevelUpMultiplier: %f"), CurrentSettings.LevelUpMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("CofferMeterReductionRate: %f"), CurrentSettings.CofferMeterReductionRate);
    UE_LOG(LogTemp, Warning, TEXT("EquipmentBonusAmout: %f"), CurrentSettings.EquipmentBonusAmout);
    UE_LOG(LogTemp, Warning, TEXT("EssenceYieldAmount: %f"), CurrentSettings.EssenceYieldAmount);
    UE_LOG(LogTemp, Warning, TEXT("LegendaryExpAmount: %f"), CurrentSettings.LegendaryExpAmount);
    UE_LOG(LogTemp, Warning, TEXT("TreeCutTimeMin: %f"), CurrentSettings.TreeCutTimeMin);
    UE_LOG(LogTemp, Warning, TEXT("TreeCutTimeMax: %f"), CurrentSettings.TreeCutTimeMax);
    UE_LOG(LogTemp, Warning, TEXT("TreeRespawnDelay: %f"), CurrentSettings.TreeRespawnDelay);
    UE_LOG(LogTemp, Warning, TEXT("CofferDropOffTime: %f"), CurrentSettings.CofferDropOffTime);
}

void ATestManager::BeginPlay()
{
    Super::BeginPlay();

    if (!TestManagerSingletonInstance)
    {
        TestManagerSingletonInstance = this;
    }
}

void ATestManager::BeginDestroy()
{
    Super::BeginDestroy();

    ResetInstance();
}

ATestManager* ATestManager::GetInstance(UWorld* World)
{
    if (!TestManagerSingletonInstance)
    {
        for (TActorIterator<ATestManager> It(World); It; ++It)
        {
            TestManagerSingletonInstance = *It;
            break;
        }
        if (!TestManagerSingletonInstance)
        {
            TestManagerSingletonInstance = World->SpawnActor<ATestManager>();
        }
    }
    return TestManagerSingletonInstance;
}

void ATestManager::ResetInstance()
{
    TestManagerSingletonInstance = nullptr;
}


