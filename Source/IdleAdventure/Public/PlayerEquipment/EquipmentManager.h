

#pragma once

#include "CoreMinimal.h"
#include <DataRegistrySource_DataTable.h>
#include "UObject/NoExportTypes.h"
#include "EquipmentManager.generated.h"



USTRUCT(BlueprintType)
struct FEquipmentData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Data", meta = (AllowPrivateAccess = "true"))
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Data", meta = (AllowPrivateAccess = "true"))
	float LevelRequired;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Data", meta = (AllowPrivateAccess = "true"))
	USkeletalMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Data", meta = (AllowPrivateAccess = "true"))
	FName SocketName;

	FEquipmentData()
		: LevelRequired(0.0f)
		, Mesh(nullptr)
		, SocketName(NAME_None)
	{
	}
};

UCLASS(Blueprintable)
class IDLEADVENTURE_API UEquipmentManager : public UObject
{
	GENERATED_BODY()
	
public:
	static UEquipmentManager& Get();

	UFUNCTION()
	void UnlockEquipment(float PlayerLevel);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEquipmentData> UnlockedEquipment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEquipmentData EquippedItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Data")
	UDataTable* AllEquipmentDataTable;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentUnlocked, const FEquipmentData&, NewEquipment);

	UPROPERTY(BlueprintAssignable)
	FOnEquipmentUnlocked OnEquipmentUnlocked;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	TArray<FEquipmentData> GetUnlockedEquipment() const;

private:

	UEquipmentManager();
};
