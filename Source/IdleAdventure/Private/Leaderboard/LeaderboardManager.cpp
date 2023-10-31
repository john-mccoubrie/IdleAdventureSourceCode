#include "Leaderboard/LeaderboardManager.h"
#include "PlayFab.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"


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
        CachedWorld->GetTimerManager().SetTimer(TimerHandle_FetchLeaderboardData, this, &ULeaderboardManager::FetchAllLeaderboards, 5.0f, false);
    }
}
/*
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
*/

void ULeaderboardManager::FetchLeaderboardDataForStatistic(const FLeaderboardDetails& Details)
{
    PlayFab::ClientModels::FGetLeaderboardAroundPlayerRequest Request;
    Request.StatisticName = Details.StatisticName;
    Request.MaxResultsCount = 7;

    clientAPI->GetLeaderboardAroundPlayer(Request,
        PlayFab::UPlayFabClientAPI::FGetLeaderboardAroundPlayerDelegate::CreateLambda(Details.SuccessCallback),
        PlayFab::FPlayFabErrorDelegate::CreateLambda(Details.ErrorCallback)
    );
}

void ULeaderboardManager::FetchAllLeaderboards()
{
    FLeaderboardDetails GlobalDetails = {
        "EXP",
        [this](const PlayFab::ClientModels::FGetLeaderboardAroundPlayerResult& Result) { OnGetPlayerLeaderboardPositionSuccess(Result, "Global"); },
        [this](const PlayFab::FPlayFabCppError& ErrorResult) { OnGetPlayerLeaderboardPositionError(ErrorResult); },
        "Global"
    };

    FLeaderboardDetails WeeklyDetails = {
        "WeeklyEXP",
        [this](const PlayFab::ClientModels::FGetLeaderboardAroundPlayerResult& Result) { OnGetPlayerLeaderboardPositionSuccess(Result, "Weekly"); },
        [this](const PlayFab::FPlayFabCppError& ErrorResult) { OnGetPlayerLeaderboardPositionError(ErrorResult); },
        "Weekly"
    };

    FetchLeaderboardDataForStatistic(GlobalDetails);
    FetchLeaderboardDataForStatistic(WeeklyDetails);
}

void ULeaderboardManager::UpdateLeaderboard()
{

}

void ULeaderboardManager::OnGetPlayerLeaderboardPositionSuccess(const PlayFab::ClientModels::FGetLeaderboardAroundPlayerResult& Result, const FString& LeaderboardType)
{
    // Players need to set a display name
    TArray<FPlayerLeaderboardData> LeaderboardDataArray;

    for (const auto& PlayerData : Result.Leaderboard)
    {
        FPlayerLeaderboardData DataEntry;
        DataEntry.PlayerName = PlayerData.DisplayName;
        DataEntry.Rank = PlayerData.Position + 1;  // 0-based to 1-based
        DataEntry.Exp = PlayerData.StatValue;

        LeaderboardDataArray.Add(DataEntry);
    }

    if (LeaderboardType == "Global")
    {
        OnGlobalLeaderboardDataReceived.Broadcast(LeaderboardDataArray);
    }
    else if (LeaderboardType == "Weekly")
    {
        OnWeeklyLeaderboardDataReceived.Broadcast(LeaderboardDataArray);
    }
}

void ULeaderboardManager::OnGetPlayerLeaderboardPositionError(const PlayFab::FPlayFabCppError& ErrorResult) const
{
    UE_LOG(LogTemp, Error, TEXT("Error retrieving leaderboard position: %s"), *ErrorResult.GenerateErrorReport());
}


