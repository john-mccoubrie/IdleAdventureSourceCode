

#pragma once

#include "EquipmentManager.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerEquipment.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class IDLEADVENTURE_API UPlayerEquipment : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerEquipment();


	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	UEquipmentManager* EquipmentManager;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void EquipItem(const FEquipmentData& ItemData);
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool CanEquipItem(const FEquipmentData& ItemData);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void UnequipCurrentItem();

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void EquipDefaultItem(const FEquipmentData& ItemData);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void ApplyEquipmentEffects(const FEquipmentData& ItemData);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void RemoveEquipmentEffects();

	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	USkeletalMeshComponent* EquippedItemMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* DefaultStaffMesh;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
