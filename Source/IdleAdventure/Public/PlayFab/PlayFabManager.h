

#pragma once

#include <PlayerEquipment/EquipmentManager.h>
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include <PlayFab.h>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayFabManager.generated.h"


/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API APlayFabManager : public AActor
{
	GENERATED_BODY()


public:

	APlayFabManager();
	virtual void BeginPlay() override;
	//Singleton pattern
	static APlayFabManager* GetInstance();

	UFUNCTION(BlueprintCallable, Category = "PlayFab")
	bool PurchaseEquipment(const FString& EquipmentName, const FEquipmentData& EquipmentData);
	bool UpdatePlayFabEssenceCount(const FEquipmentData& EquipmentData);
	bool UpdatePlayerEquipmentInventory(const TArray<FEquipmentData>& NewPlayerEquipmentInventory);

	//Update PlayFab
	bool TryPlayFabUpdate(FName EssenceType, int32 NewCount);
	void OnSuccessUpdateEssence(const PlayFab::ClientModels::FUpdateUserDataResult& Result);
	void OnErrorUpdateEssence(const PlayFab::FPlayFabCppError& Error);
	TSharedPtr<FJsonObject> TMapToJsonObject(const TMap<FName, int32>& Map);
	bool UpdateEssenceAddedToCofferOnPlayFab();
	void InitializeEssenceCounts();

private:

	static APlayFabManager* GPlayFabManager;
	PlayFabClientPtr clientAPI = nullptr;

	// Static variable to hold the singleton instance
	static APlayFabManager* SingletonInstance;
	
};
