

#pragma once

#include <PlayerEquipment/EquipmentManager.h>
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include <PlayFab.h>
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PlayFabManager.generated.h"


/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API UPlayFabManager : public UObject
{
	GENERATED_BODY()


public:

	UPlayFabManager();
	//Singleton pattern
	
	static UPlayFabManager* GetInstance();

	UFUNCTION(BlueprintCallable, Category = "PlayFab")
	bool PurchaseEquipment(const FString& EquipmentName, const FEquipmentData& EquipmentData);
	bool UpdatePlayFabEssenceCount(const FEquipmentData& EquipmentData);
	bool UpdatePlayerEquipmentInventory(const TArray<FEquipmentData>& NewPlayerEquipmentInventory);

	//Update PlayFab
	bool TryPlayFabUpdate(FName EssenceType, int32 NewCount);
	void OnSuccessUpdateEssence(const PlayFab::ClientModels::FUpdateUserDataResult& Result);
	void OnErrorUpdateEssence(const PlayFab::FPlayFabCppError& Error);

private:

	static UPlayFabManager* GPlayFabManager;
	PlayFabClientPtr clientAPI = nullptr;
	
};
