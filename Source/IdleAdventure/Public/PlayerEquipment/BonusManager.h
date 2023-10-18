

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BonusManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_EightParams(FOnBonusesUpdated, float, WisdomEssenceMultiplier, 
    float, TemperanceEssenceMultiplier, float, JusticeEssenceMultiplier, float, CourageEssenceMultiplier, 
    int32, WisdomYieldMultiplier, int32, TemperanceYieldMultiplier, int32, JusticeYieldMultiplier, int32, CourageYieldMultiplier);

USTRUCT(BlueprintType)
struct FItemBonus
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ExperienceMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WisdomEssenceMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TemperanceEssenceMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float JusticeEssenceMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CourageWisdomEssenceMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LegendaryEssenceMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int WisdomYieldMultiplier = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int TemperanceYieldMultiplier = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int JusticeYieldMultiplier = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int CourageYieldMultiplier = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int LegendaryYieldMultiplier = 1;

    bool operator==(const FItemBonus& Other) const
    {
        return WisdomEssenceMultiplier == Other.WisdomEssenceMultiplier &&
            TemperanceEssenceMultiplier == Other.TemperanceEssenceMultiplier &&
            JusticeEssenceMultiplier == Other.JusticeEssenceMultiplier &&
            CourageWisdomEssenceMultiplier == Other.CourageWisdomEssenceMultiplier &&
            WisdomYieldMultiplier == Other.WisdomYieldMultiplier &&
            TemperanceYieldMultiplier == Other.TemperanceYieldMultiplier &&
            JusticeYieldMultiplier == Other.JusticeYieldMultiplier &&
            CourageYieldMultiplier == Other.CourageYieldMultiplier;
    }

};

UCLASS()
class IDLEADVENTURE_API ABonusManager : public AActor
{
	GENERATED_BODY()

public:

    ABonusManager();
    virtual void BeginPlay() override;
    virtual void BeginDestroy() override;
    static ABonusManager* GetInstance(UWorld* World);
    void ResetInstance();
    
    void ApplyExperienceBonus(float Multiplier);
    void ApplyEssenceBonus(const FItemBonus& ItemBonus);
    void RemoveEssenceBonus(const FItemBonus& ItemBonus);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ItemIdentifier;

    float WisdomEssenceMultiplier;
    float TemperanceEssenceMultiplier;
    float JusticeEssenceMultiplier;
    float CourageEssenceMultiplier;
    float LegendaryEssenceMultiplier;

    int WisdomYieldMultiplier;
    int TemperanceYieldMultiplier;
    int JusticeYieldMultiplier;
    int CourageYieldMultiplier;
    int LegendaryYieldMultiplier;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bonuses")
    FOnBonusesUpdated OnBonusesUpdated;

private:
    static ABonusManager* BonusManagerSingletonInstance;
    TArray<FItemBonus> ActiveBonuses;
};
