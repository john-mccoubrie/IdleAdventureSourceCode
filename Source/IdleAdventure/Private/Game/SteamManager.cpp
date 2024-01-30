


#include "Game/SteamManager.h"
#include "EngineUtils.h"
#define ELobbyType Steam_ELobbyType
#include "steam/steam_api.h"
#undef ELobbyType
//#include <mfapi.h>
//#include "steam/SteamLeaderboard.h"

ASteamManager* ASteamManager::SteamManagerSingletonInstance = nullptr;

ASteamManager::ASteamManager()
{
 	

}


void ASteamManager::BeginPlay()
{
	Super::BeginPlay();
	if (!SteamManagerSingletonInstance)
	{
		SteamManagerSingletonInstance = this;
	}

    if (SteamAPI_Init()) {
        // Steam API is initialized
        //g_SteamLeaderboards = new CSteamLeaderboards();
        DownloadScores("EasyMap");
    }
    else {
        // Handle error, Steam API not available
    }
}

void ASteamManager::BeginDestroy()
{
	Super::BeginDestroy();
	ResetInstance();
}

void ASteamManager::Tick(float DeltaTime)
{
    SteamAPI_RunCallbacks();
}

ASteamManager* ASteamManager::GetInstance(UWorld* World)
{
    if (!SteamManagerSingletonInstance)
    {
        for (TActorIterator<ASteamManager> It(World); It; ++It)
        {
            SteamManagerSingletonInstance = *It;
            break;
        }
        if (!SteamManagerSingletonInstance)
        {
            SteamManagerSingletonInstance = World->SpawnActor<ASteamManager>();
        }
    }
    return SteamManagerSingletonInstance;
}

void ASteamManager::ResetInstance()
{
    SteamManagerSingletonInstance = nullptr;
}

bool ASteamManager::IsSteamAchievementUnlocked(const FString& AchievementID)
{
    bool bAchieved = false;
    if (SteamAPI_Init())
    {
        SteamUserStats()->GetAchievement(TCHAR_TO_ANSI(*AchievementID), &bAchieved);
    }
    return bAchieved;
}

void ASteamManager::UnlockSteamAchievement(const FString& AchievementID)
{
    if (!IsSteamAchievementUnlocked(AchievementID))
    {
        if (SteamAPI_Init())
        {
            SteamUserStats()->SetAchievement(TCHAR_TO_ANSI(*AchievementID));
            SteamUserStats()->StoreStats();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Steam API is not initialized in steam manager"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Steam acheivement is already unlocked!"));
    }
}


FString ASteamManager::GetSteamName()
{
    FString SteamName = UTF8_TO_TCHAR(SteamFriends()->GetPersonaName());
    return SteamName;
}

void ASteamManager::FindLeaderboard(const char* pchLeaderboardName)
{
    m_CurrentLeaderboard = NULL;

    SteamAPICall_t hSteamAPICall = SteamUserStats()->FindLeaderboard(pchLeaderboardName);
    m_callResultFindLeaderboard.Set(hSteamAPICall, this,
        &ASteamManager::OnFindLeaderboard);
}

bool ASteamManager::UploadScore(const FString& LeaderboardName, int score)
{
    CurrentOperation = ELeaderboardOperation::UploadScore;
    // Convert FString to const char*
    const char* CharLeaderboardName = TCHAR_TO_UTF8(*LeaderboardName);

    // Find the leaderboard first
    m_CurrentLeaderboard = NULL;
    m_PendingScore = score; // Store the score to upload once the leaderboard is found
    SteamAPICall_t hFindLeaderboardCall = SteamUserStats()->FindLeaderboard(CharLeaderboardName);
    m_callResultFindLeaderboard.Set(hFindLeaderboardCall, this, &ASteamManager::OnFindLeaderboard);

    return true; // Return true to indicate the process has started
}

bool ASteamManager::DownloadScores(const FString& LeaderboardName)
{
    UE_LOG(LogTemp, Warning, TEXT("Download score called"));
    CurrentOperation = ELeaderboardOperation::DownloadScores;
    // Convert FString to const char*
    const char* CharLeaderboardName = TCHAR_TO_UTF8(*LeaderboardName);

    // Find the leaderboard first
    SteamAPICall_t hFindLeaderboardCall = SteamUserStats()->FindLeaderboard(CharLeaderboardName);
    m_callResultFindLeaderboard.Set(hFindLeaderboardCall, this, &ASteamManager::OnFindLeaderboard);

    return true; // Return true to indicate the process has started
}

void ASteamManager::OnFindLeaderboard(LeaderboardFindResult_t* pResult, bool bIOFailure)
{
    if (bIOFailure || (pResult && !pResult->m_bLeaderboardFound))
    {
        UE_LOG(LogTemp, Warning, TEXT("Leaderboard could not be found!"));
        return;
    }

    m_CurrentLeaderboard = pResult->m_hSteamLeaderboard;

    if (m_CurrentLeaderboard)
    {
        // Declare hSteamAPICall outside the switch
        SteamAPICall_t hSteamAPICall;

        switch (CurrentOperation)
        {
        case ELeaderboardOperation::UploadScore:
            if (m_PendingScore >= 0) // Assuming score is a non-negative value
            {
                hSteamAPICall = SteamUserStats()->UploadLeaderboardScore(m_CurrentLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, m_PendingScore, NULL, 0);
                m_callResultUploadScore.Set(hSteamAPICall, this, &ASteamManager::OnUploadScore);
                UE_LOG(LogTemp, Log, TEXT("Uploading score: %d"), m_PendingScore);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Invalid score for upload"));
            }
            break;

        case ELeaderboardOperation::DownloadScores:
            hSteamAPICall = SteamUserStats()->DownloadLeaderboardEntries(m_CurrentLeaderboard, k_ELeaderboardDataRequestGlobalAroundUser, -4, 5); // Adjust range as needed
            m_callResultDownloadScore.Set(hSteamAPICall, this, &ASteamManager::OnDownloadScore);
            UE_LOG(LogTemp, Log, TEXT("Downloading leaderboard scores"));
            break;

        default:
            UE_LOG(LogTemp, Log, TEXT("Default/None called for OnFindLeaderboard"));
            break;
        }
    }
}

void ASteamManager::OnUploadScore(LeaderboardScoreUploaded_t* pResult, bool bIOFailure)
{
    if (bIOFailure || (pResult && !pResult->m_bSuccess))
    {
        UE_LOG(LogTemp, Warning, TEXT("Score could not be uploaded to Steam"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Upload Success"));
    }
}

void ASteamManager::OnDownloadScore(LeaderboardScoresDownloaded_t* pResult, bool bIOFailure)
{
    TArray<FSteamLeaderboardEntry> LeaderboardEntries;
    CSteamID CurrentPlayerSteamID = SteamUser()->GetSteamID();
    FSteamLeaderboardEntry CurrentPlayerEntry;

    UE_LOG(LogTemp, Log, TEXT("OnDownloadScore called. IO Failure: %d"), bIOFailure);


    if (!bIOFailure && pResult)
    {
        UE_LOG(LogTemp, Log, TEXT("Processing %d entries."), pResult->m_cEntryCount);
        m_nLeaderboardEntries = FMath::Min(pResult->m_cEntryCount, 10);

        for (int index = 0; index < m_nLeaderboardEntries; index++)
        {
            LeaderboardEntry_t entry;
            int detailsArray[1];
            if (SteamUserStats()->GetDownloadedLeaderboardEntry(
                pResult->m_hSteamLeaderboardEntries, index, &entry, detailsArray, 1))
            {
                FSteamLeaderboardEntry NewEntry;
                NewEntry.PlayerName = FString(SteamFriends()->GetFriendPersonaName(entry.m_steamIDUser));
                NewEntry.Rank = entry.m_nGlobalRank;
                //int score = detailsArray[0];
                int score = entry.m_nScore; //directly access score
                UE_LOG(LogTemp, Log, TEXT("Score details: %i"), score);
                FString TempScore = GetFormattedTime(score);
                NewEntry.Score = TempScore;

                LeaderboardEntries.Add(NewEntry);

                // Check if this entry is the current player
                if (entry.m_steamIDUser == CurrentPlayerSteamID)
                {
                    CurrentPlayerEntry = NewEntry;
                }
            }
        }

        AsyncTask(ENamedThreads::GameThread, [=]()
            {
                OnLeaderboardDataReceived.Broadcast(LeaderboardEntries);
            });
        

        // Broadcast current player's data if available
        if (CurrentPlayerEntry.PlayerName != "")
        {
            AsyncTask(ENamedThreads::GameThread, [=]()
                {
                    OnCurrentPlayerLeaderboardDataReceived.Broadcast(CurrentPlayerEntry);
                });
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to download leaderboard scores or pResult is NULL"));
    }
}

FString ASteamManager::GetFormattedTime(int TimeInSeconds)
{
    int TotalSeconds = static_cast<int>(TimeInSeconds);
    int Minutes = TotalSeconds / 60;
    int Seconds = TotalSeconds % 60;

    return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}

/*
void ASteamManager::UploadScoreToLeaderboard(const FString& LeaderboardName, int32 Score)
{
    if (SteamAPI_Init())
    {
        SteamAPICall_t hSteamAPICall = SteamUserStats()->UploadLeaderboardScore(
            FindLeaderboard(LeaderboardName), k_ELeaderboardUploadScoreMethodKeepBest, Score, nullptr, 0);

        // Register for the call result
        m_SteamCallResultUploadScore.Set(hSteamAPICall, this, &ASteamManager::OnUploadScoreToLeaderboard);

        UE_LOG(LogTemp, Warning, TEXT("UploadScoreToLeaderboardCalled"));

        LeaderboardNameLocal = LeaderboardName;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Steam API is not initialized"));
    }
}

void ASteamManager::OnDownloadLeaderboardEntries(LeaderboardScoresDownloaded_t* pCallback, bool bIOFailure)
{
    TArray<int32> LeaderboardScores;
    // Process the downloaded scores and update the UI accordingly
    if (!bIOFailure)
    {
        for (int index = 0; index < pCallback->m_cEntryCount; ++index)
        {
            LeaderboardEntry_t leaderboardEntry;
            int32 details[1]; // Assuming we have 1 detail integer
            if (SteamUserStats()->GetDownloadedLeaderboardEntry(pCallback->m_hSteamLeaderboardEntries, index, &leaderboardEntry, details, 1))
            {
                // Assuming the detail is the score you want
                LeaderboardScores.Add(details[0]);
            }
        }

        //OnLeaderboardScoresDownloaded.Broadcast(true, LeaderboardNameLocal, LeaderboardScores);
    }
    else
    {
        //OnLeaderboardScoresDownloaded.Broadcast(false, LeaderboardNameLocal, LeaderboardScores);
        UE_LOG(LogTemp, Warning, TEXT("Failed to download leaderboard entries."));
    }
}

void ASteamManager::OnUploadScoreToLeaderboard(LeaderboardScoreUploaded_t* pCallback, bool bIOFailure)
{
    bool bWasSuccessful = pCallback->m_bSuccess && !bIOFailure;
    OnLeaderboardScoreUploaded.Broadcast(bWasSuccessful, LeaderboardNameLocal);
    UE_LOG(LogTemp, Warning, TEXT("Successfully uploaded to Steam Leaderboard"));
}

SteamLeaderboard_t ASteamManager::FindLeaderboard(const FString& LeaderboardName)
{
    if (SteamAPI_Init())
    {
        return SteamUserStats()->FindLeaderboard(TCHAR_TO_ANSI(*LeaderboardName));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Steam API is not initialized in steam manager"));
        return false;
    }
}
*/

