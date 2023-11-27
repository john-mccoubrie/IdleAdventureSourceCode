

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BonusManager.h"
#include "UObject/NoExportTypes.h"
#include "EquipmentManager.generated.h"


USTRUCT(BlueprintType)
struct FEquipmentData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Data", meta = (AllowPrivateAccess = "true"))
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Data", meta = (AllowPrivateAccess = "true"))
	FString ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Data", meta = (AllowPrivateAccess = "true"))
	float LevelRequired;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Data", meta = (AllowPrivateAccess = "true"))
	USkeletalMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Data", meta = (AllowPrivateAccess = "true"))
	FName SocketName;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Data", meta = (AllowPrivateAccess = "true"))
    int32 WisdomCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Data", meta = (AllowPrivateAccess = "true"))
    int32 TemperanceCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Data", meta = (AllowPrivateAccess = "true"))
    int32 JusticeCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Data", meta = (AllowPrivateAccess = "true"))
    int32 CourageCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Data", meta = (AllowPrivateAccess = "true"))
    int32 LegendaryCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Data", meta = (AllowPrivateAccess = "true"))
	FItemBonus ItemBonus;
	
    FEquipmentData()
        : Name(TEXT(""))
		, ItemDescription(TEXT(""))
		, LevelRequired(0.0f)
        , Mesh(nullptr)
		, Icon(nullptr)
        , SocketName(NAME_None)
        , WisdomCost(0)
        , TemperanceCost(0)
        , JusticeCost(0)
        , CourageCost(0)
        , LegendaryCost(0)
		, ItemBonus()
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
