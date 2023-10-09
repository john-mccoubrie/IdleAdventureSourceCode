

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

	TArray<ACoffer*> AllCoffers;
	TArray<ACoffer*> ActiveCoffers;

private:
	static ACofferManager* CofferManagerSingletonInstance;



	

};
