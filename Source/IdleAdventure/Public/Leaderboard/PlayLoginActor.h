
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <PlayFab.h>
#include "IdleGameInstance.h"
#include "Leaderboard/UpdatePlayerDisplayName.h"
#include "Chat/PhotonChatManager.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include "PlayLoginActor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExpLoaded, int32, LoadedExp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeeklyExpLoaded, int32, WeeklyLoadedExp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerLevelLoaded, int32, LoadedPlayerLevel);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLeaderboardDataReceived, int32, Player);

class AIdleGameModeBase;

UCLASS()
class IDLEADVENTURE_API APlayLoginActor : public AActor
{
	GENERATED_BODY()
	
public:
    APlayLoginActor();
    virtual void BeginPlay() override;

    //UFUNCTION(BlueprintCallable, Category = "PlayFab||Login")
    //void LoginWithSteam();


    void OnSuccess(const PlayFab::ClientModels::FLoginResult& Result);
    void OnError(const PlayFab::FPlayFabCppError& ErrorResult) const;

    virtual void Tick(float DeltaSeconds) override;

    void SavePlayerStatsToPlayFab(const FString& PlayerName, int32 GlobalExp, int32 WeeklyExp, int32 PlayerLevel);

    void OnGetLeaderboardSuccess(const PlayFab::ClientModels::FUpdatePlayerStatisticsResult& result) const;
    void OnGetLeaderboardError(const PlayFab::FPlayFabCppError& ErrorResult) const;

    void LoadWoodcuttingExpFromPlayFab();
    void OnLoadExpSuccess(const PlayFab::ClientModels::FGetPlayerStatisticsResult& result);
    void OnLoadExpError(const PlayFab::FPlayFabCppError& ErrorResult) const;
    void GetPlayerLeaderboardPosition();
    void OnGetPlayerLeaderboardPositionSuccess(const PlayFab::ClientModels::FGetLeaderboardAroundPlayerResult& Result);
    void OnGetPlayerLeaderboardPositionError(const PlayFab::FPlayFabCppError& ErrorResult) const;

    int32 CurrentWeeklyExp = 0;

    UPROPERTY(BlueprintAssignable, Category = "PlayFab")
    FOnExpLoaded OnExpLoaded;

    UPROPERTY(BlueprintAssignable, Category = "PlayFab")
    FOnWeeklyExpLoaded OnWeeklyExpLoaded;

    UPROPERTY(BlueprintAssignable, Category = "PlayFab")
    FOnPlayerLevelLoaded OnPlayerLevelLoaded;

    //UPROPERTY(BlueprintAssignable, Category = "PlayFab")
    //FOnLeaderboardDataReceived OnLeaderboardDataReceived;

    UFUNCTION(BlueprintCallable, Category = "PlayFab||Login")
    void RegisterUser(FString username, FString password, FString email);

    UFUNCTION(BlueprintCallable, Category = "PlayFab||Login")
    void LoginUser(FString username, FString password, FString email);

    

    void OnRegisterSuccess(const PlayFab::ClientModels::FRegisterPlayFabUserResult& result) const;
    void OnLoginSuccess(const PlayFab::ClientModels::FLoginResult& result);
    void OnLoginError(const PlayFab::FPlayFabCppError& errorResult) const;

    UFUNCTION()
    void HandleChatConnected();

    UFUNCTION()
    void HandleUpdateDisplayNameSuccess();

    //APhotonChatManager* PhotonChatManagerInstance;
    AIdleGameModeBase* IdleGameMode;
    UIdleGameInstance* IdleGameInstance;

    UPROPERTY(EditAnywhere, Category = "Lifecycle")
    bool bShouldConnectToChatAfterLevelLoad = false;
    ExitGames::Common::JString PendingUserID;

    UPROPERTY(BlueprintReadOnly, Category = "PlayFab")
    UUpdatePlayerDisplayName* UpdatePlayerDisplayNameInstance;

    PlayFabClientPtr clientAPI = nullptr;

    //Photon Chat Authentication
    void OnGetPhotonTokenSuccess(const PlayFab::ClientModels::FGetPhotonAuthenticationTokenResult& Result);
    void OnGetPhotonTokenError(const PlayFab::FPlayFabCppError& ErrorResult);
    void AuthenticateWithPhoton(const FString& PhotonToken);


    //Steam
    UFUNCTION(BlueprintCallable, Category = "Steam||Login")
    void LoginWithSteam();
    void OnLoginSteamSuccess(const PlayFab::ClientModels::FLoginResult& Result);
    void OnLoginSteamError(const PlayFab::FPlayFabCppError& ErrorResult);
    void UpdateDisplayNameFromSteam(const FString& DesiredDisplayName);
    void OnUpdateDisplayNameSuccess(const PlayFab::ClientModels::FUpdateUserTitleDisplayNameResult& Result);
    void OnUpdateDisplayNameError(const PlayFab::FPlayFabCppError& Error);
    void LoadLevelSelectScreen();
    void AuthenticateWithPhoton();

};
