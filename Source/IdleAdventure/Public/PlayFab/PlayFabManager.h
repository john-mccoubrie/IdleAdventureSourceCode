

#pragma once

#include <PlayerEquipment/EquipmentManager.h>
#include <Character/IdleCharacter.h>
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include <PlayFab.h>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayFabManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPurchaseCompleted, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEssenceTransferredPlayFab, const TArray<FEssenceCoffer>&, EssenceCofferArray);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryLoaded, const TArray<FName>&, InventoryRowNames);
/**
 * 
 */
UCLASS(Blueprintable)
class IDLEADVENTURE_API APlayFabManager : public AActor
{
	GENERATED_BODY()


public:

	APlayFabManager();
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	//Singleton pattern
	static APlayFabManager* GetInstance(UWorld* World);
	void ResetInstance();

	UFUNCTION(BlueprintCallable, Category = "PlayFab")
	bool PurchaseEquipment(const FString& EquipmentName, const FEquipmentData& EquipmentData);
	bool UpdatePlayFabEssenceCount(const FEquipmentData& EquipmentData);
	bool UpdatePlayerEquipmentInventory(const TArray<FEquipmentData>& NewPlayerEquipmentInventory);
	void LoadPlayerEquipmentInventory();
	void OnSuccessFetchInventory(const PlayFab::ClientModels::FGetUserDataResult& Result);
	void OnErrorFetchInventory(const PlayFab::FPlayFabCppError& Error);

	//Update and fetch data to/from PlayFab
	bool TryPlayFabUpdate(FName EssenceType, int32 NewCount);
	void OnSuccessUpdateEssence(const PlayFab::ClientModels::FUpdateUserDataResult& Result);
	void OnErrorUpdateEssence(const PlayFab::FPlayFabCppError& Error);
	void OnSuccessUpdateInventory(const PlayFab::ClientModels::FUpdateUserDataResult& Result);
	void OnErrorUpdateInventory(const PlayFab::FPlayFabCppError& Error);
	TSharedPtr<FJsonObject> TMapToJsonObject(const TMap<FName, int32>& Map);
	bool UpdateEssenceAddedToCofferOnPlayFab();
	void InitializeEssenceCounts();
	//PlayFab helper classes
	FString ConvertToPlayFabFormat(const TArray<FEquipmentData>& EquipmentDataArray);
	TArray<FName> ConvertFromPlayFabFormat(const FString& PlayFabData);

	//Delegate broadcasts to UI
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPurchaseCompleted OnPurchaseCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnEssenceTransferredPlayFab OnEssenceTransferred;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryLoaded OnInventoryLoaded;

	AIdleCharacter* Character;

	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	TArray<FName> DataTableRowNames;

private:

	PlayFabClientPtr clientAPI = nullptr;

	// Static variable to hold the singleton instance
	static APlayFabManager* SingletonInstance;

	//Only broadcast to the UI once, keep track of a count
	int32 SuccessfulUpdateCount;
	
};
