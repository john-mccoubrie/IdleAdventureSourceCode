

#pragma once

#include "CoreMinimal.h"
#include "Actor/Base_NPCActor.h"
#include <Chat/GameChatManager.h>
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include <PlayFab.h>
#include "NPC_Investor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractedWithInvestor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnEssenceUpdate, int32, Wisdom, int32, Temperance, int32, Justice, int32, Courage, int32, Legendary);



USTRUCT(BlueprintType)
struct FInvestmentLossResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 WisdomLoss;

	UPROPERTY(BlueprintReadOnly)
	int32 TemperanceLoss;

	UPROPERTY(BlueprintReadOnly)
	int32 JusticeLoss;

	UPROPERTY(BlueprintReadOnly)
	int32 LegendaryLoss;

	FInvestmentLossResult()
		: WisdomLoss(0), TemperanceLoss(0), JusticeLoss(0), LegendaryLoss(0) {}
};


UCLASS()
class IDLEADVENTURE_API ANPC_Investor : public ABase_NPCActor
{
	GENERATED_BODY()

public:

	virtual void Interact() override;
	virtual void BeginPlay() override;

	//Investment logic
	UFUNCTION(BlueprintCallable, Category = "Investing")
	int32 HandleInvest(int32 PlayerWisdomAmt, int32 PlayerTemperanceAmt, int32 PlayerJusticeAmt, int32 PlayerLegendaryAmt); //takes in struct or essence types
	int32 GetEquipmentBonus();
	int32 CalculateCourageEssenceReturn(int32 Points);
	float CalculateSuccessChance(int32 TotalInvestment, int32 EquipmentBonus);
	FInvestmentLossResult HandleInvestmentLoss(int32 PlayerWisdomAmt, int32 PlayerTemperanceAmt, int32 PlayerJusticeAmt, int32 PlayerLegendaryAmt);
	void UpdatePlayerEssenceCounts(int32 WisdomAmt, int32 TemperanceAmt, int32 JusticeAmt, int32 LegendaryAmt, int32 CourageEssence, bool bIsSuccessful);

	//Playfab calls
	void GetCurrentEssenceCounts();
	void OnSuccessFetchEssenceCounts(const PlayFab::ClientModels::FGetUserDataResult& Result);
	void OnErrorFetchEssenceCounts(const PlayFab::FPlayFabCppError& Error);
	void SendUpdatedEssenceCountsToPlayFab();
	void OnSuccessUpdateEssenceCounts(const PlayFab::ClientModels::FUpdateUserDataResult& Result);
	void OnErrorUpdateEssenceCounts(const PlayFab::FPlayFabCppError& Error);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInteractedWithInvestor OnInteractedWithInvestor;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEssenceUpdate OnEssenceUpdate;

	//PlayFab stored
	int32 PlayFabWisdom;
	int32 PlayFabTemperance;
	int32 PlayFabJustice;
	int32 PlayFabCourage;
	int32 PlayFabLegendary;

private:
	PlayFabClientPtr clientAPI = nullptr;
	
};
