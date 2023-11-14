
#include "Leaderboard/UpdatePlayerDisplayName.h"
#include "PlayFab.h"
#include "IdleGameInstance.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include <Kismet/GameplayStatics.h>
#include <Game/IdleGameModeLogin.h>


UUpdatePlayerDisplayName::UUpdatePlayerDisplayName()
{
    clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
}

UUpdatePlayerDisplayName::~UUpdatePlayerDisplayName()
{

}

void UUpdatePlayerDisplayName::UpdateDisplayName(const FString& DesiredDisplayName)
{
    PlayFab::ClientModels::FUpdateUserTitleDisplayNameRequest DisplayNameRequest;
    DisplayNameRequest.DisplayName = DesiredDisplayName;

    clientAPI->UpdateUserTitleDisplayName(DisplayNameRequest,
        PlayFab::UPlayFabClientAPI::FUpdateUserTitleDisplayNameDelegate::CreateUObject(this, &UUpdatePlayerDisplayName::OnUpdateDisplayNameSuccess),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UUpdatePlayerDisplayName::OnUpdateDisplayNameError)
    );
}

void UUpdatePlayerDisplayName::FetchPlayerProfile(const FString& PlayFabId)
{
    PlayFab::ClientModels::FGetPlayerProfileRequest ProfileRequest;
    ProfileRequest.PlayFabId = PlayFabId;

    clientAPI->GetPlayerProfile(ProfileRequest,
        PlayFab::UPlayFabClientAPI::FGetPlayerProfileDelegate::CreateUObject(this, &UUpdatePlayerDisplayName::OnGetPlayerProfileSuccess),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &UUpdatePlayerDisplayName::OnGetPlayerProfileError)
    );
}

void UUpdatePlayerDisplayName::OnUpdateDisplayNameSuccess(const PlayFab::ClientModels::FUpdateUserTitleDisplayNameResult& Result)
{
    UE_LOG(LogTemp, Warning, TEXT("Display name updated successfully!"));
    AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
    GameChatManager->PostNotificationToLoginScreen(TEXT("Player name updated successfully!"), FLinearColor::Green);
    //Display name is successfully updated, delegate points to APlayLoginActor to call "Idle Forest" Level
    //Cannot call GetWorld() here because it is a UObject and has no world context
    OnUpdateDisplayNameSuccessDelegate.Broadcast();
}

void UUpdatePlayerDisplayName::OnUpdateDisplayNameError(const PlayFab::FPlayFabCppError& Error)
{
    UE_LOG(LogTemp, Error, TEXT("Error updating display name: %s"), *Error.ErrorMessage);
    AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
    GameChatManager->PostNotificationToLoginScreen(TEXT("Player name updated successfully!"), FLinearColor::Red);
}

void UUpdatePlayerDisplayName::OnGetPlayerProfileSuccess(const PlayFab::ClientModels::FGetPlayerProfileResult& Result)
{
    bHasDisplayName = !Result.PlayerProfile.Get()->DisplayName.IsEmpty();
    TestDisplayName = Result.PlayerProfile.Get()->DisplayName;
    if (bHasDisplayName)
    {
        // If you have a display name, set it as the Photon nickname.
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APhotonChatManager::StaticClass(), FoundActors);

        // Check if we found any instances
        if (FoundActors.Num() > 0)
        {
            // Cast the first found actor to APhotonChatManager and call the function
            APhotonChatManager* PhotonChatManager = Cast<APhotonChatManager>(FoundActors[0]);
            if (PhotonChatManager)
            {
                //PhotonChatManager->ConnectToChat(jPhotonToken);
                PhotonChatManager->SetPhotonNickNameToPlayFabDisplayName(Result.PlayerProfile.Get()->DisplayName);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No DisplayName Set!"));
    }
}

void UUpdatePlayerDisplayName::OnGetPlayerProfileError(const PlayFab::FPlayFabCppError& Error)
{
    UE_LOG(LogTemp, Error, TEXT("Error fetching player profile: %s"), *Error.ErrorMessage);
}

