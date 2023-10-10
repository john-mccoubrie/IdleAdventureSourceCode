

#pragma once

#include <PlayFab.h>
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include "CoreMinimal.h"
#include "Item.h"
#include "GameFramework/Actor.h"
#include "Inventory.generated.h"

USTRUCT(BlueprintType)
struct FEssenceCoffer
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FName Key;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int32 Value;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAdded, FName, RowName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, FName, RowName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FboolOnItemAdded, bool, bItemAdded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEssenceTransferred, const TArray<FEssenceCoffer>&, EssenceCofferArray);


UCLASS()
class IDLEADVENTURE_API AInventory : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInventory();

    virtual void BeginPlay() override;

    // Add an item to the inventory
    UFUNCTION(BlueprintCallable, Category = "Inventory")
        void AddItem(UItem* Item);

    // Remove an item from the inventory
    UFUNCTION(BlueprintCallable, Category = "Inventory")
        void RemoveItem(UItem* Item);

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
        FOnItemAdded OnItemAdded;

        UPROPERTY(BlueprintAssignable, Category = "Inventory")
        FOnItemRemoved OnItemRemoved;

        UPROPERTY(BlueprintAssignable, Category = "Inventory")
        FboolOnItemAdded boolOnItemAdded;

        //UPROPERTY(BlueprintAssignable, Category = "Events")
        //FOnEssenceUpdated OnEssenceUpdated;

        UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TMap<FName, int32> EssenceCount;

        UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
        TMap<FName, int32> EssenceAddedToCoffer;

        UFUNCTION(BlueprintCallable, Category = "Inventory")
        void TransferAndClearEssenceCounts();

        UPROPERTY(BlueprintAssignable, Category = "Inventory")
        FOnEssenceTransferred OnEssenceTransferred;

        //PlayFab
        void OnGetDataSuccess(const PlayFab::ClientModels::FGetUserDataResult& Result);
        void OnGetDataFailure(const PlayFab::FPlayFabCppError& Error);
        void OnUpdateDataSuccess(const PlayFab::ClientModels::FUpdateUserDataResult& Result);
        void OnUpdateDataFailure(const PlayFab::FPlayFabCppError& Error);
        void RequestPlayFabData();
        void InitializeEssenceCounts();

        TSharedPtr<FJsonObject> TMapToJsonObject(const TMap<FName, int32>& Map);
        TMap<FName, int32> JsonObjectToTMap(const TSharedPtr<FJsonObject>& JsonObject);

        PlayFabClientPtr clientAPI = nullptr;

        float EssenceCountForDurationCalc;


    // List of items in the inventory
    UPROPERTY(EditAnywhere, Category = "Inventory")
        TArray<UItem*> Items;

        int32 GetEssenceCount(FName EssenceType) const;

        TMap<FName, int32> GetAllEssenceCounts() const;

};
