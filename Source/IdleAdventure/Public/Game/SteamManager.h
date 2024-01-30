

#pragma once

#include "CoreMinimal.h"
#include <steam/isteamuserstats.h>
//#include "steam/SteamLeaderboards.h"
//#include "steam/steamleaderboards.h"
#include "GameFramework/Actor.h"
#include "SteamManager.generated.h"


USTRUCT(BlueprintType)
struct FSteamLeaderboardEntry
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString PlayerName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Rank;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Score;

	FSteamLeaderboardEntry()
		: PlayerName("")
		, Rank(0)
		, Score("")
	{
	}
};

UENUM(BlueprintType)
enum class ELeaderboardOperation : uint8
{
	None,
	UploadScore,
	DownloadScores
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSteamLeaderboardDataReceived, const TArray<FSteamLeaderboardEntry>&, LeaderboardData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentPlayerLeaderboardDataReceived, const FSteamLeaderboardEntry&, CurrentPlayerData);

UCLASS()
class IDLEADVENTURE_API ASteamManager : public AActor
{
	GENERATED_BODY()
	
public:
	ASteamManager();
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaTime) override;
	static ASteamManager* GetInstance(UWorld* World);
	void ResetInstance();

	bool IsSteamAchievementUnlocked(const FString& AchievementID);
	void UnlockSteamAchievement(const FString& AchievementID);
	FString GetSteamName();


	/*
	// Steam Leaderboard functions
	void UploadScoreToLeaderboard(const FString& LeaderboardName, int32 Score);
	void DownloadLeaderboardEntries(const FString& LeaderboardName);
	SteamLeaderboard_t FindLeaderboard(const FString& LeaderboardName);

	// Steam Leaderboard callbacks
	void OnUploadScoreToLeaderboard(LeaderboardScoreUploaded_t* pCallback, bool bIOFailure);
	void OnDownloadLeaderboardEntries(LeaderboardScoresDownloaded_t* pCallback, bool bIOFailure);
	// Delegates that other parts of the game can bind to
	UPROPERTY(BlueprintAssignable, Category = "Steam|Leaderboards")
	FOnLeaderboardScoreUploaded OnLeaderboardScoreUploaded;

	//UPROPERTY(BlueprintAssignable, Category = "Steam|Leaderboards")
	//FOnLeaderboardScoresDownloaded OnLeaderboardScoresDownloaded;

	CCallResult<ASteamManager, LeaderboardScoreUploaded_t> m_SteamCallResultUploadScore;
	CCallResult<ASteamManager, LeaderboardScoresDownloaded_t> m_SteamCallResultDownloadScores;

	//CSteamLeaderboards* g_SteamLeaderboards = NULL;
	*/

	void FindLeaderboard(const char* pchLeaderboardName);
	bool UploadScore(const FString& LeaderboardName, int score);
	UFUNCTION(BlueprintCallable, Category = "Steam Leaderboard")
	bool DownloadScores(const FString& LeaderboardName);

	void OnFindLeaderboard(LeaderboardFindResult_t* pResult, bool bIOFailure);
	CCallResult<ASteamManager, LeaderboardFindResult_t> m_callResultFindLeaderboard;
	void OnUploadScore(LeaderboardScoreUploaded_t* pResult, bool bIOFailure);
	CCallResult<ASteamManager, LeaderboardScoreUploaded_t> m_callResultUploadScore;
	void OnDownloadScore(LeaderboardScoresDownloaded_t* pResult, bool bIOFailure);
	CCallResult<ASteamManager, LeaderboardScoresDownloaded_t> m_callResultDownloadScore;

	int m_nLeaderboardEntries; // How many entries do we have?
	LeaderboardEntry_t m_leaderboardEntries[10];

	UPROPERTY(BlueprintAssignable, Category = "Steam Leaderboard")
	FOnSteamLeaderboardDataReceived OnLeaderboardDataReceived;
	UPROPERTY(BlueprintAssignable, Category = "Steam Leaderboard")
	FOnCurrentPlayerLeaderboardDataReceived OnCurrentPlayerLeaderboardDataReceived;

	FString GetFormattedTime(int TimeInSeconds);

	ELeaderboardOperation CurrentOperation = ELeaderboardOperation::None;

private:
	static ASteamManager* SteamManagerSingletonInstance;
	FString LeaderboardNameLocal;
	SteamLeaderboard_t m_CurrentLeaderboard; // Handle to leaderboard
	int m_PendingScore;

};
