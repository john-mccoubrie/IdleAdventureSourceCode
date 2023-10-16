

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
    float CourageWisdomEssenceMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int WisdomYieldMultiplier = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int TemperanceYieldMultiplier = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int JusticeYieldMultiplier = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int CourageYieldMultiplier = 1;

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
    void RemoveEssenceBonus();

    float WisdomEssenceMultiplier;
    float TemperanceEssenceMultiplier;
    float JusticeEssenceMultiplier;
    float CourageEssenceMultiplier;

    int WisdomYieldMultiplier;
    int TemperanceYieldMultiplier;
    int JusticeYieldMultiplier;
    int CourageYieldMultiplier;

private:
    static ABonusManager* BonusManagerSingletonInstance;

};
