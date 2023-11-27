
#include "Test/TestManager.h"
#include "EngineUtils.h"
#include <Engine/DataTable.h>

ATestManager* ATestManager::TestManagerSingletonInstance = nullptr;

// Sets default values
ATestManager::ATestManager()
{
    UDataTable* GatheringSettingsDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("/Game/Blueprints/DataTables/DT_GatheringGameSettings.DT_GatheringGameSettings")));

    FName RowKey = "GatheringGameSettings";
    FGameSettings* InvestingValues = GatheringSettingsDataTable->FindRow<FGameSettings>(RowKey, TEXT("Context String"), true);

    // Set default values for the game settings
    if (InvestingValues)
    {
        CurrentSettings.EssenceYieldSpeed = InvestingValues->EssenceYieldSpeed;
        CurrentSettings.LevelUpMultiplier = InvestingValues->LevelUpMultiplier;
        CurrentSettings.CofferMeterReductionRate = InvestingValues->CofferMeterReductionRate;
        CurrentSettings.EquipmentBonusAmout = InvestingValues->EquipmentBonusAmout;
        CurrentSettings.EssenceYieldAmount = InvestingValues->EssenceYieldAmount;
        CurrentSettings.LegendaryExpAmount = InvestingValues->LegendaryExpAmount;
        CurrentSettings.TreeCutTimeMin = InvestingValues->TreeCutTimeMin;
        CurrentSettings.TreeCutTimeMax = InvestingValues->TreeCutTimeMax;
        CurrentSettings.TreeRespawnDelay = InvestingValues->TreeRespawnDelay;
        CurrentSettings.CofferDropOffTime = InvestingValues->CofferDropOffTime;
        CurrentSettings.EssenceAddToCofferMultiply = InvestingValues->EssenceAddToCofferMultiply;
        CurrentSettings.LogYieldFrequencyFactor = InvestingValues->LogYieldFrequencyFactor;
    } 
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


