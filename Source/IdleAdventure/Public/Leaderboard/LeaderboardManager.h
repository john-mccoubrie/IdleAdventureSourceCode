

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

	// Methods to interact with PlayFab
	void FetchLeaderboardData();
	void UpdateLeaderboard();

	//UFUNCTION()
	void OnGetPlayerLeaderboardPositionSuccess(const PlayFab::ClientModels::FGetLeaderboardAroundPlayerResult& Result);

	void OnGetPlayerLeaderboardPositionError(const PlayFab::FPlayFabCppError& ErrorResult) const;

	UPROPERTY(BlueprintAssignable, Category = "PlayFab")
	FOnLeaderboardDataReceived OnLeaderboardDataReceived;

private:
	PlayFabClientPtr clientAPI = nullptr;
	FTimerHandle TimerHandle_FetchLeaderboardData;

	void StartTimer();
	UWorld* CachedWorld = nullptr;
};
