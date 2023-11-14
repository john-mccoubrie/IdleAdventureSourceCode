

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
    float ExperienceMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WisdomEssenceMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TemperanceEssenceMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float JusticeEssenceMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CourageEssenceMultiplier = 1.0f;

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WisdomEssenceChanceBonus = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TemperanceEssenceChanceBonus = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float JusticeEssenceChanceBonus = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CourageEssenceChanceBonus = 0.0f;

    bool operator==(const FItemBonus& Other) const
    {
        return WisdomEssenceMultiplier == Other.WisdomEssenceMultiplier &&
            TemperanceEssenceMultiplier == Other.TemperanceEssenceMultiplier &&
            JusticeEssenceMultiplier == Other.JusticeEssenceMultiplier &&
            CourageEssenceMultiplier == Other.CourageEssenceMultiplier &&
            WisdomYieldMultiplier == Other.WisdomYieldMultiplier &&
            TemperanceYieldMultiplier == Other.TemperanceYieldMultiplier &&
            JusticeYieldMultiplier == Other.JusticeYieldMultiplier &&
            CourageYieldMultiplier == Other.CourageYieldMultiplier &&
            WisdomEssenceChanceBonus == Other.WisdomEssenceChanceBonus &&
            TemperanceEssenceChanceBonus == Other.TemperanceEssenceChanceBonus &&
            JusticeEssenceChanceBonus == Other.JusticeEssenceChanceBonus &&
            CourageEssenceChanceBonus == Other.CourageEssenceChanceBonus;
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

    float WisdomEssenceChanceMultiplier;
    float TemperanceEssenceChanceMultiplier;
    float JusticeEssenceChanceMultiplier;
    float CourageEssenceChanceMultiplier;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bonuses")
    FOnBonusesUpdated OnBonusesUpdated;

private:
    static ABonusManager* BonusManagerSingletonInstance;
    TArray<FItemBonus> ActiveBonuses;
};
