


#include "Leaderboard/PlayLoginActor.h"
//#include "Chat/PhotonChatManager.h"
#include "Game/IdleGameModeBase.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include <Kismet/GameplayStatics.h>
#include <IdleGameInstance.h>

// Sets default values
APlayLoginActor::APlayLoginActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayLoginActor::BeginPlay()
{
	Super::BeginPlay();
	
    GetMutableDefault<UPlayFabRuntimeSettings>()->TitleId = TEXT("8B4AE");

    clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

    //Update display name reference and delegate when complete
    UpdatePlayerDisplayNameInstance = NewObject<UUpdatePlayerDisplayName>();
    UpdatePlayerDisplayNameInstance->OnUpdateDisplayNameSuccessDelegate.AddDynamic(this, &APlayLoginActor::HandleUpdateDisplayNameSuccess);


    if (bShouldConnectToChatAfterLevelLoad)
    {
        AIdleGameModeBase* GameMode = Cast<AIdleGameModeBase>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            GameMode->ConnectToChat(PendingUserID);
            //UE_LOG(LogTemp, Warning, TEXT("Connected to chat called after level load"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("From BeginPlay GameMode is null ptr"));
        }
    }
}

void APlayLoginActor::OnSuccess(const PlayFab::ClientModels::FLoginResult& Result)
{
    UE_LOG(LogTemp, Log, TEXT("Congratulations, you made your first successful API call!"));
    LoadWoodcuttingExpFromPlayFab();
}

void APlayLoginActor::OnError(const PlayFab::FPlayFabCppError& ErrorResult) const
{
    UE_LOG(LogTemp, Error, TEXT("Something went wrong with your first API call.\nHere's some debug information:\n%s"), *ErrorResult.GenerateErrorReport());
}

// Called every frame
void APlayLoginActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
}

void APlayLoginActor::UpdateLeaderboard(const FString& PlayerName, int32 WoodcuttingExp, int32 PlayerLevel)
{
    //UE_LOG(LogTemp, Warning, TEXT("Update Leaderboard called"));
    
    //PlayFabClientPtr ClientPtr = IPlayFabModuleInterface::Get().GetClientAPI();

    //Update PlayerExp
    PlayFab::ClientModels::FStatisticUpdate ExpStatisticsUpdate;
    ExpStatisticsUpdate.StatisticName = "EXP";
    ExpStatisticsUpdate.Value = WoodcuttingExp;

    //Update PlayerLevel
    PlayFab::ClientModels::FStatisticUpdate PlayerStatisticsUpdate;
    PlayerStatisticsUpdate.StatisticName = "PlayerLevel";
    PlayerStatisticsUpdate.Value = PlayerLevel;

    PlayFab::ClientModels::FUpdatePlayerStatisticsRequest Request;
    Request.Statistics.Add(ExpStatisticsUpdate);
    Request.Statistics.Add(PlayerStatisticsUpdate);

    clientAPI->UpdatePlayerStatistics(Request,
        PlayFab::UPlayFabClientAPI::FUpdatePlayerStatisticsDelegate::CreateUObject(this, &APlayLoginActor::OnGetLeaderboardSuccess),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &APlayLoginActor::OnGetLeaderboardError)
    );
}



void APlayLoginActor::OnGetLeaderboardSuccess(const PlayFab::ClientModels::FUpdatePlayerStatisticsResult& result) const
{
    //UE_LOG(LogTemp, Warning, TEXT("Update Successful!"));
}

void APlayLoginActor::OnGetLeaderboardError(const PlayFab::FPlayFabCppError& ErrorResult) const
{
    UE_LOG(LogTemp, Warning, TEXT("Failed!"));
}

void APlayLoginActor::LoadWoodcuttingExpFromPlayFab()
{
    //PlayFabClientPtr ClientPtr = IPlayFabModuleInterface::Get().GetClientAPI();

    PlayFab::ClientModels::FGetPlayerStatisticsRequest Request;
    Request.StatisticNames.Add("EXP");
    Request.StatisticNames.Add("PlayerLevel");

    clientAPI->GetPlayerStatistics(Request,
        PlayFab::UPlayFabClientAPI::FGetPlayerStatisticsDelegate::CreateUObject(this, &APlayLoginActor::OnLoadExpSuccess),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &APlayLoginActor::OnLoadExpError)
    );
}

void APlayLoginActor::OnLoadExpSuccess(const PlayFab::ClientModels::FGetPlayerStatisticsResult& Result) const
{
    int32 LoadedExp = 0;
    int32 LoadedPlayerLevel = 0;

    for (const auto& Stat : Result.Statistics)
    {
        if (Stat.StatisticName == "EXP")
        {
            LoadedExp = Stat.Value;
        }
        else if (Stat.StatisticName == "PlayerLevel")
        {
            LoadedPlayerLevel = Stat.Value;
        }
    }

    // Now you have both LoadedExp and LoadedPlayerLevel
    // You can use them as needed, for example:
    OnExpLoaded.Broadcast(LoadedExp);
    OnPlayerLevelLoaded.Broadcast(LoadedPlayerLevel); // Assuming you have a similar delegate for PlayerLevel
}

void APlayLoginActor::OnLoadExpError(const PlayFab::FPlayFabCppError& ErrorResult) const
{
    UE_LOG(LogTemp, Warning, TEXT("Failed to load WoodcuttingExp from PlayFab!"));
}

void APlayLoginActor::GetPlayerLeaderboardPosition()
{
    //PlayFab::ClientModels::FGetLeaderboardAroundPlayerRequest Request;
    //Request.StatisticName = "EXP";  // This is the name of the leaderboard you want to retrieve.
    //Request.MaxResultsCount = 1;    // You want just the data around the current player.

    //clientAPI->GetLeaderboardAroundPlayer(Request,
        //PlayFab::UPlayFabClientAPI::FGetLeaderboardAroundPlayerDelegate::CreateUObject(this, &APlayLoginActor::OnGetPlayerLeaderboardPositionSuccess),
        //PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &APlayLoginActor::OnGetPlayerLeaderboardPositionError)
    //);
}

void APlayLoginActor::OnGetPlayerLeaderboardPositionSuccess(const PlayFab::ClientModels::FGetLeaderboardAroundPlayerResult& Result)
{
    /*
    if (Result.Leaderboard.Num() > 0)
    {
        //int32 PlayerRank = Result.Leaderboard[0].Position;
        //OnLeaderboardDataReceived.Broadcast(PlayerRank + 1);
        //DisplayPlayerRank(PlayerRank + 1);  // Assuming DisplayPlayerRank displays the rank in your UI.
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to retrieve player's leaderboard position."));
    }
    */
}

void APlayLoginActor::OnGetPlayerLeaderboardPositionError(const PlayFab::FPlayFabCppError& ErrorResult) const
{
    // Handle any errors that occur when trying to retrieve the leaderboard position.
    UE_LOG(LogTemp, Error, TEXT("Error retrieving leaderboard position: %s"), *ErrorResult.GenerateErrorReport());
}

void APlayLoginActor::RegisterUser(FString username, FString password, FString email)
{
    PlayFab::ClientModels::FRegisterPlayFabUserRequest RegisterRequest;
    RegisterRequest.Username = username;
    RegisterRequest.Password = password;
    RegisterRequest.Email = email;

    // Set to false for testing
    //RegisterRequest.RequireBothUsernameAndEmail = false;

    //PlayFabClientPtr clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

    clientAPI->PlayFab::UPlayFabClientAPI::RegisterPlayFabUser(RegisterRequest,
        PlayFab::UPlayFabClientAPI::FRegisterPlayFabUserDelegate::CreateUObject(this, &APlayLoginActor::OnRegisterSuccess),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &APlayLoginActor::OnLoginError));
}

void APlayLoginActor::LoginUser(FString username, FString password, FString email)
{
    PlayFab::ClientModels::FLoginWithPlayFabRequest LoginRequest;
    LoginRequest.Username = username;
    LoginRequest.Password = password;

    if (clientAPI.IsValid())
    {
        clientAPI->LoginWithPlayFab(LoginRequest,
            PlayFab::UPlayFabClientAPI::FLoginWithPlayFabDelegate::CreateUObject(this, &APlayLoginActor::OnLoginSuccess),
            PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &APlayLoginActor::OnLoginError)
        );
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("clientAPI is not valid!"));
    }
}

void APlayLoginActor::OnRegisterSuccess(const PlayFab::ClientModels::FRegisterPlayFabUserResult& result) const
{
    UE_LOG(LogTemp, Warning, TEXT("Player registered successfully!"));
}

void APlayLoginActor::OnLoginSuccess(const PlayFab::ClientModels::FLoginResult& result)
{
    UE_LOG(LogTemp, Warning, TEXT("Player login successful: %s"), *result.PlayFabId);
    //UGameplayStatics::OpenLevel(this, TEXT("Sandbox"));
    FString PlayFabUserID = result.PlayFabId; // store the playfab user ID
    std::wstring wstrUserID(*PlayFabUserID);

    // Convert std::wstring to ExitGames::Common::JString
    ExitGames::Common::JString jUserID = wstrUserID.c_str();

    bShouldConnectToChatAfterLevelLoad = true;
    PendingUserID = jUserID;
    IdleGameInstance = Cast<UIdleGameInstance>(GetGameInstance());
    if (IdleGameInstance)
    {
        IdleGameInstance->StoredLoginData = jUserID;
        UE_LOG(LogTemp, Warning, TEXT("Game instance stored data"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("IdleGameInstance is null"));
    }

    //check if the player has a display name set

    

    if (UpdatePlayerDisplayNameInstance)
    {
        UpdatePlayerDisplayNameInstance->FetchPlayerProfile(PlayFabUserID);
        FTimerHandle ProfileFetchTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            ProfileFetchTimerHandle,
            [this]() {
                if (UpdatePlayerDisplayNameInstance->HasDisplayName())
                {
                    UGameplayStatics::OpenLevel(this, TEXT("IdleForest"));
                }
                else
                {
                    UGameplayStatics::OpenLevel(this, TEXT("ProfileSetup"));
                }
            },
            1.0f,  // Wait for 1 second (adjust as needed)
            false
        );
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create UpdatePlayerDisplayName instance."));
    }

    //UGameplayStatics::OpenLevel(this, TEXT("IdleForest"));
}

void APlayLoginActor::OnLoginError(const PlayFab::FPlayFabCppError& errorResult) const
{
   //FString errorMessage = FString::Printf(TEXT("PlayFab Error: %s"), *errorResult.GenerateErrorReport());

    if (GEngine)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, errorMessage);
    }
    //UE_LOG(LogTemp, Error, TEXT("%s"), *errorMessage);
}

void APlayLoginActor::HandleChatConnected()
{
    
}

void APlayLoginActor::HandleUpdateDisplayNameSuccess()
{
    UGameplayStatics::OpenLevel(this, TEXT("IdleForest"));
}


