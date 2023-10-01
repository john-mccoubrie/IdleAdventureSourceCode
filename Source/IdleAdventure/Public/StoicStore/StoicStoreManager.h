

#pragma once

#include <PlayerEquipment/EquipmentManager.h>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StoicStoreManager.generated.h"


UCLASS(Blueprintable)
class IDLEADVENTURE_API AStoicStoreManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStoicStoreManager();
	
	UFUNCTION(BlueprintCallable, Category = "Store")
	bool PurchaseItem(const FEquipmentData& ItemData);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
