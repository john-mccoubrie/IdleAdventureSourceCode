

#pragma once

#include "Coffer.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CofferManager.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CofferManager")
	TArray<ACoffer*> AllCoffers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CofferManager")
	TArray<ACoffer*> ActiveCoffers;

private:
	static ACofferManager* CofferManagerSingletonInstance;
};
