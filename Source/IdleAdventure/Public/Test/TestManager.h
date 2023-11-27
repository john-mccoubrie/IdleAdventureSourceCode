

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestManager.generated.h"

USTRUCT(BlueprintType)
struct FGameSettings : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float EssenceYieldSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float LevelUpMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float CofferMeterReductionRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float EquipmentBonusAmout;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float EssenceYieldAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float LegendaryExpAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float TreeCutTimeMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float TreeCutTimeMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float TreeRespawnDelay;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float CofferDropOffTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float EssenceAddToCofferMultiply;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float LogYieldFrequencyFactor;

	FGameSettings()
	: EssenceYieldSpeed(1.0f)
	, LevelUpMultiplier(1.50f)
	, CofferMeterReductionRate(1.0f)
	, EquipmentBonusAmout(1.0f)
	, EssenceYieldAmount(1.0f)
	, LegendaryExpAmount(1.0f)
	, TreeCutTimeMin(15.0f)
	, TreeCutTimeMax(45.0f)
	, TreeRespawnDelay(500.0f)
	, CofferDropOffTime(10.0f)
	, EssenceAddToCofferMultiply(10.0f)
	, LogYieldFrequencyFactor(1.0f)

	{
	}
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSettingsChanged, FGameSettings, NewSettings);

UCLASS()
class IDLEADVENTURE_API ATestManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestManager();
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ChangeSettings(FGameSettings NewSettings);
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	static ATestManager* GetInstance(UWorld* World);
	void ResetInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FGameSettings CurrentSettings;

	UPROPERTY(BlueprintAssignable, Category = "Settings")
	FOnSettingsChanged OnSettingsChanged;

private:
	static ATestManager* TestManagerSingletonInstance;
};
