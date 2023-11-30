

#pragma once

#include "CoreMinimal.h"
#include "Actor/Base_NPCActor.h"
#include <Chat/GameChatManager.h>
#include "Engine/DataTable.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include <PlayFab.h>
#include "NPC_Investor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractedWithInvestor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBroadcastInvestNumbers, int32, Points, int32, EquipmentBonus, float, SuccessChance);



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


USTRUCT(BlueprintType)
struct FInvestingValues : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investing")
	float BaseSuccessChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investing")
	float WisdomInvestmentMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investing")
	float TemperanceInvestmentMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investing")
	float JusticeInvestmentMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investing")
	float LegendaryInvestmentMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investing")
	float PowerFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investing")
	float LossPercentage;

	// Constructor for defaults
	FInvestingValues()
		: BaseSuccessChance(0.2f)
		, WisdomInvestmentMultiplier(0.5f)
		, TemperanceInvestmentMultiplier(0.75f)
		, JusticeInvestmentMultiplier(1.0f)
		, LegendaryInvestmentMultiplier(1.5f)
		, PowerFactor(1.0f)
		, LossPercentage(1.0f)
	{
	}
};


UCLASS()
class IDLEADVENTURE_API ANPC_Investor : public ABase_NPCActor
{
	GENERATED_BODY()

public:
	ANPC_Investor();

	virtual void Interact() override;
	virtual void BeginPlay() override;

	//Investment logic
	UFUNCTION(BlueprintCallable, Category = "Investing")
	int32 HandleInvest(int32 PlayerWisdomAmt, int32 PlayerTemperanceAmt, int32 PlayerJusticeAmt, int32 PlayerLegendaryAmt); //takes in struct or essence types
	int32 GetEquipmentBonus();
	int32 CalculateCourageEssenceReturn(int32 Points);
	float CalculateSuccessChance(int32 PlayerWisdomAmt, int32 PlayerTemperanceAmt, int32 PlayerJusticeAmt, int32 PlayerLegendaryAmt, int32 EquipmentBonus);
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
	FOnBroadcastInvestNumbers OnBroadcastInvestNumbers;

	//PlayFab stored
	int32 PlayFabWisdom;
	int32 PlayFabTemperance;
	int32 PlayFabJustice;
	int32 PlayFabCourage;
	int32 PlayFabLegendary;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investing")
	float BaseSuccessChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investing")
	float WisdomInvestmentMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investing")
	float TemperanceInvestmentMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investing")
	float JusticeInvestmentMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investing")
	float LegendaryInvestmentMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investing")
	float PowerFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Investing")
	float LossPercentage;

private:
	PlayFabClientPtr clientAPI = nullptr;
	
};
