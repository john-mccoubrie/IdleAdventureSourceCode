

#pragma once

#include "CoreMinimal.h"
#include <PlayFab.h>
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include "LeaderboardManager.generated.h"

USTRUCT(BlueprintType)
struct FPlayerLeaderboardData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString PlayerName;

	UPROPERTY(BlueprintReadOnly)
	int32 Rank;

	UPROPERTY(BlueprintReadOnly)
	int32 Exp;

	FPlayerLeaderboardData()
		: Rank(0)
		, Exp(0)
	{
	}
};

struct FLeaderboardDetails
{
	FString StatisticName;
	TFunction<void(const PlayFab::ClientModels::FGetLeaderboardAroundPlayerResult&)> SuccessCallback;
	TFunction<void(const PlayFab::FPlayFabCppError&)> ErrorCallback;
	FString LeaderboardType;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLeaderboardDataReceived, const TArray<FPlayerLeaderboardData>&, PlayFabLeaderboardData);
/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API ULeaderboardManager : public UObject
{
	GENERATED_BODY()

public:
	ULeaderboardManager();
	~ULeaderboardManager();

	void Initialize(UWorld* InWorld) { CachedWorld = InWorld; StartTimer(); }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leaderboard Data", meta = (AllowPrivateAccess = "true"))
	int32 Rank = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leaderboard Data", meta = (AllowPrivateAccess = "true"))
	int32 Exp = 0;

	// Methods to interact with PlayFab
	void FetchLeaderboardDataForStatistic(const FLeaderboardDetails& Details);
	void FetchAllLeaderboards();
	void UpdateLeaderboard();

	//UFUNCTION()
	void OnGetPlayerLeaderboardPositionSuccess(const PlayFab::ClientModels::FGetLeaderboardAroundPlayerResult& Result, const FString& LeaderboardType);

	void OnGetPlayerLeaderboardPositionError(const PlayFab::FPlayFabCppError& ErrorResult) const;

	UPROPERTY(BlueprintAssignable, Category = "PlayFab")
	FOnLeaderboardDataReceived OnGlobalLeaderboardDataReceived;

	UPROPERTY(BlueprintAssignable, Category = "PlayFab")
	FOnLeaderboardDataReceived OnWeeklyLeaderboardDataReceived;

private:
	PlayFabClientPtr clientAPI = nullptr;
	FTimerHandle TimerHandle_FetchLeaderboardData;

	void StartTimer();
	UWorld* CachedWorld = nullptr;
};
