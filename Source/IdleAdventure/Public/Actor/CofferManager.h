

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

	TArray<ACoffer*> AllCoffers;
	TArray<ACoffer*> ActiveCoffers;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
