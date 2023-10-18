

#pragma once

#include "Coffer.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CofferManager.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActiveCofferCountChanged, ACoffer*, ClickedCoffer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCofferClicked, float, ProgressRatio, float, TotalTime, float, RemainingTime);

UCLASS()
class IDLEADVENTURE_API ACofferManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACofferManager();
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	static ACofferManager* GetInstance(UWorld* World);
	void ResetInstance();
	void SetUpAllCoffers();
	bool CheckIfCofferIsActive(ACoffer* CofferToCheck);
	int32 CheckNumOfActiveCoffers();
	void AddActiveCoffer(ACoffer* NewCoffer);
	void RemoveActiveCoffer(ACoffer* CofferToRemove);
	void UpdateProgressBar(ACoffer* UpdatedCoffer, float ProgressRatio);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CofferManager")
	TArray<ACoffer*> AllCoffers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CofferManager")
	TArray<ACoffer*> ActiveCoffers;

	UPROPERTY(BlueprintAssignable, Category = "Coffer")
	FOnActiveCofferCountChanged OnActiveCofferCountChanged;

	UPROPERTY(BlueprintAssignable)
	FOnCofferClicked OnCofferClicked;

	int32 ProgressBarIndex;

	TMap<ACoffer*, int32> CofferProgressBarMapping;

	void StartExperienceTimer(float Duration);
	void DecrementExperienceTime();
	FTimerHandle ExperienceTimerHandle;
	float TotalExperienceTime;
	float RemainingExperienceTime;

private:
	static ACofferManager* CofferManagerSingletonInstance;
};
