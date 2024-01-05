

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BonusManager.generated.h"

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
    int WisdomYieldMultiplier = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int TemperanceYieldMultiplier = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int JusticeYieldMultiplier = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int CourageYieldMultiplier = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int LegendaryYieldMultiplier = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WisdomEssenceChanceBonus = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TemperanceEssenceChanceBonus = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float JusticeEssenceChanceBonus = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CourageEssenceChanceBonus = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 InvestingBonusChance = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Armor = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Speed = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString WeaponTypeIdentifier;

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
            CourageEssenceChanceBonus == Other.CourageEssenceChanceBonus &&
            InvestingBonusChance == Other.InvestingBonusChance &&
            Damage == Other.Damage &&
            Armor == Other.Armor &&
            Speed == Other.Speed &&
            WeaponTypeIdentifier == Other.WeaponTypeIdentifier;
    }

};

USTRUCT(BlueprintType)
struct FMultiplierSet
{
    GENERATED_BODY()

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
    int32 WisdomYieldMultiplier = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TemperanceYieldMultiplier = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 JusticeYieldMultiplier = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CourageYieldMultiplier = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 LegendaryYieldMultiplier = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WisdomEssenceChanceMultiplier = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TemperanceEssenceChanceMultiplier = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float JusticeEssenceChanceMultiplier = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CourageEssenceChanceMultiplier = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 InvestingBonusChance = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Armor = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Speed = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString WeaponTypeIdentifier = "None";

    void ApplyBonus(const FItemBonus& ItemBonus)
    {
        WisdomEssenceMultiplier *= ItemBonus.WisdomEssenceMultiplier;
        TemperanceEssenceMultiplier *= ItemBonus.TemperanceEssenceMultiplier;
        JusticeEssenceMultiplier *= ItemBonus.JusticeEssenceMultiplier;
        CourageEssenceMultiplier *= ItemBonus.CourageEssenceMultiplier;
        LegendaryEssenceMultiplier *= ItemBonus.LegendaryEssenceMultiplier;

        WisdomYieldMultiplier += ItemBonus.WisdomYieldMultiplier;
        TemperanceYieldMultiplier += ItemBonus.TemperanceYieldMultiplier;
        JusticeYieldMultiplier += ItemBonus.JusticeYieldMultiplier;
        CourageYieldMultiplier += ItemBonus.CourageYieldMultiplier;
        LegendaryYieldMultiplier += ItemBonus.LegendaryYieldMultiplier;

        WisdomEssenceChanceMultiplier += ItemBonus.WisdomEssenceChanceBonus;
        TemperanceEssenceChanceMultiplier += ItemBonus.TemperanceEssenceChanceBonus;
        JusticeEssenceChanceMultiplier += ItemBonus.JusticeEssenceChanceBonus;
        CourageEssenceChanceMultiplier += ItemBonus.CourageEssenceChanceBonus;

        InvestingBonusChance += ItemBonus.InvestingBonusChance;

        Damage += ItemBonus.Damage;
        Armor += ItemBonus.Armor;
        Speed += ItemBonus.Speed;
        WeaponTypeIdentifier = ItemBonus.WeaponTypeIdentifier;
    }

    void RemoveBonus(const FItemBonus& ItemBonus)
    {
        WisdomEssenceMultiplier /= ItemBonus.WisdomEssenceMultiplier;
        TemperanceEssenceMultiplier /= ItemBonus.TemperanceEssenceMultiplier;
        JusticeEssenceMultiplier /= ItemBonus.JusticeEssenceMultiplier;
        CourageEssenceMultiplier /= ItemBonus.CourageEssenceMultiplier;
        LegendaryEssenceMultiplier /= ItemBonus.LegendaryEssenceMultiplier;

        WisdomYieldMultiplier -= ItemBonus.WisdomYieldMultiplier;
        TemperanceYieldMultiplier -= ItemBonus.TemperanceYieldMultiplier;
        JusticeYieldMultiplier -= ItemBonus.JusticeYieldMultiplier;
        CourageYieldMultiplier -= ItemBonus.CourageYieldMultiplier;
        LegendaryYieldMultiplier -= ItemBonus.LegendaryYieldMultiplier;

        WisdomEssenceChanceMultiplier -= ItemBonus.WisdomEssenceChanceBonus;
        TemperanceEssenceChanceMultiplier -= ItemBonus.TemperanceEssenceChanceBonus;
        JusticeEssenceChanceMultiplier -= ItemBonus.JusticeEssenceChanceBonus;
        CourageEssenceChanceMultiplier -= ItemBonus.CourageEssenceChanceBonus;

        InvestingBonusChance -= ItemBonus.InvestingBonusChance;

        Damage -= ItemBonus.Damage;
        Armor -= ItemBonus.Armor;
        Speed -= ItemBonus.Speed;

        WeaponTypeIdentifier = "None";
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBroadcastToBonusUI, FItemBonus, ItemBonusStruct);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBroadcastStaff, FString, StaffName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBroadcastCape, FString, CapeName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBroadcastAura, FString, AuraName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBonusesUpdated, FMultiplierSet, MultiplierSetStruct);

UCLASS()
class IDLEADVENTURE_API ABonusManager : public AActor
{
	GENERATED_BODY(Blueprintable)

public:

    ABonusManager();
    virtual void BeginPlay() override;
    virtual void BeginDestroy() override;
    static ABonusManager* GetInstance(UWorld* World);
    void ResetInstance();
    
    void ApplyExperienceBonus(float Multiplier);
    void ApplyEssenceBonus(const FItemBonus& ItemBonus);
    void RemoveEssenceBonus(const FItemBonus& ItemBonus);
    void UpdateStaffName(FString StaffName);
    void UpdateCapeName(FString CapeName);
    void UpdateAuraName(FString AuraName);

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

    int32 InvestingBonusChance;

    float Damage;
    float Armor;

    float Speed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString WeaponTypeIdentifier;


public:
    UPROPERTY(BlueprintAssignable, Category = "Bonuses")
    FOnBonusesUpdated OnBonusesUpdated;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FMultiplierSet MultiplierSet;

    UPROPERTY(BlueprintAssignable, Category = "Bonuses")
    FOnBroadcastToBonusUI OnBroadcastToBonusUI;

    UPROPERTY(BlueprintAssignable, Category = "Bonuses")
    FOnBroadcastStaff OnBroadcastStaff;
    UPROPERTY(BlueprintAssignable, Category = "Bonuses")
    FOnBroadcastCape OnBroadcastCape;
    UPROPERTY(BlueprintAssignable, Category = "Bonuses")
    FOnBroadcastAura OnBroadcastAura;

private:
    static ABonusManager* BonusManagerSingletonInstance;
    TArray<FItemBonus> ActiveBonuses;
};
