

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

private:
    static ABonusManager* BonusManagerSingletonInstance;

};
