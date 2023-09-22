#include "PlayFab.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include "Leaderboard/LeaderboardManager.h"

ULeaderboardManager::ULeaderboardManager()
{
    clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
}

ULeaderboardManager::~ULeaderboardManager()
{
    // Clean up the timer on destruction
    //if (GetWorld())
    //{
        //GetWorld()->GetTimerManager().ClearTimer(TimerHandle_FetchLeaderboardData);
    //}
}

void ULeaderboardManager::StartTimer()
{
    if (CachedWorld)
    {
        //CachedWorld->GetTimerManager().SetTimer(TimerHandle_FetchLeaderboardData, this, &ULeaderboardManager::FetchLeaderboardData, 5.0f, true);
        CachedWorld->GetTimerManager().SetTimer(TimerHandle_FetchLeaderboardData, this, &ULeaderboardManager::FetchLeaderboardData, 5.0f, false);
    }
}

void ULeaderboardManager::FetchLeaderboardData()
{
    PlayFab::ClientModels::FGetLeaderboardAroundPlayerRequest Request;
    Request.StatisticName = "EXP";  // This is the name of the leaderboard you want to retrieve.
    Request.MaxResultsCount = 7;    // You want just the data around the current player.

    clientAPI->GetLeaderboardAroundPlayer(Request,
        PlayFab::UPlayFabClientAPI::FGetLeaderboardAroundPlayerDelegate::CreateUObject(this, &ULeaderboardManager::OnGetPlayerLeaderboardPositionSuccess),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &ULeaderboardManager::OnGetPlayerLeaderboardPositionError)
    );
}

void ULeaderboardManager::UpdateLeaderboard()
{

}

void ULeaderboardManager::OnGetPlayerLeaderboardPositionSuccess(const PlayFab::ClientModels::FGetLeaderboardAroundPlayerResult& Result)
{
    //Players need to set a display name
    TArray<FPlayerLeaderboardData> LeaderboardDataArray;
    UE_LOG(LogTemp, Warning, TEXT("Retreived leaderboard"));
    for (const auto& PlayerData : Result.Leaderboard)
    {
        FPlayerLeaderboardData DataEntry;
        DataEntry.PlayerName = PlayerData.DisplayName;
        DataEntry.Rank = PlayerData.Position + 1;  // 0-based to 1-based
        DataEntry.Exp = PlayerData.StatValue;

        LeaderboardDataArray.Add(DataEntry);
        UE_LOG(LogTemp, Warning, TEXT("Leaderboard position: %s"), *PlayerData.DisplayName);
        //UE_LOG(LogTemp, Warning, TEXT("Leaderboard position: %d"), PlayerData.Rank);
        //UE_LOG(LogTemp, Warning, TEXT("Leaderboard position: %d"), PlayerData.Exp);
    }
    
    OnLeaderboardDataReceived.Broadcast(LeaderboardDataArray);

    /*
    //UE_LOG(LogTemp, Warning, TEXT("Retreived leaderboard"));
    int32 PlayerRank = Result.Leaderboard[0].Position;
    UE_LOG(LogTemp, Warning, TEXT("Leaderboard position: %d"), PlayerRank + 1);
    OnLeaderboardDataReceived.Broadcast(PlayerRank + 1);
    */
}

void ULeaderboardManager::OnGetPlayerLeaderboardPositionError(const PlayFab::FPlayFabCppError& ErrorResult) const
{
    UE_LOG(LogTemp, Error, TEXT("Error retrieving leaderboard position: %s"), *ErrorResult.GenerateErrorReport());
}


