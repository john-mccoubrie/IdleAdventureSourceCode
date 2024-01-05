


#include "Game/SteamManager.h"
#include "EngineUtils.h"
#define ELobbyType Steam_ELobbyType
#include "steam/steam_api.h"
#undef ELobbyType

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



