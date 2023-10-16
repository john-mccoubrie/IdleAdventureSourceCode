
#include "Chat/PhotonChatManager.h"
#include "PlayFab.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include "Chat-cpp/inc/Listener.h"
#include <Chat/PhotonListener.h>
#include "EngineUtils.h"


//APhotonChatManager* APhotonChatManager::PhotonChatManagerSingletonInstance = nullptr;

APhotonChatManager::APhotonChatManager()
{
    PrimaryActorTick.bCanEverTick = true;
    clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    UE_LOG(LogTemp, Warning, TEXT("PhotonChatManager created"));
}

APhotonChatManager::~APhotonChatManager()
{
    //delete chatListener;
    //delete chatClient;
}

void APhotonChatManager::InitializeUserID(FString initUserID)
{
    UserID = ExitGames::Common::JString(*initUserID); // convert Fstring to Jstring
    //ConnectToChat(UserID);
}

void APhotonChatManager::ConnectToChat(ExitGames::Common::JString& userID, ExitGames::Common::JString& photonToken)
{
    UE_LOG(LogTemp, Warning, TEXT("PhotonChatManager connecting to chat..."));
    chatListener = new PhotonListener();
    chatListener->Owner = this;
    //GetDisplayName();

    static const ExitGames::Common::JString appID = L"abbf3a31-ccd8-4c23-8704-3ec991bc5d0b";
    static const ExitGames::Common::JString appVersion = L"1.0";
    chatClient = new ExitGames::Chat::Client(*chatListener, appID, appVersion);

    if (chatClient)
    {
        chatClient->setRegion(L"US");
        ExitGames::Common::JString params = "username=" + userID + "&token=" + photonToken;
        ExitGames::Chat::AuthenticationValues playFabAuthenticationValues;
        playFabAuthenticationValues.setType(ExitGames::Chat::CustomAuthenticationType::CUSTOM).setParameters(params);
        chatClient->connect(playFabAuthenticationValues);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("chatClient is null ptr in ConnectToChat"));
    }
}

void APhotonChatManager::Tick(float DeltaTime)
{
    if (chatClient)
    {
        chatClient->service();
        //UE_LOG(LogTemp, Warning, TEXT("chatClient->service() called"));
    }
    else
    {
        //UE_LOG(LogTemp, Warning, TEXT("chatClient is null ptr in Tick"));
    }
    
}
//Singleton Setup
/*

void APhotonChatManager::BeginPlay()
{
    Super::BeginPlay();
    if (!PhotonChatManagerSingletonInstance)
    {
        PhotonChatManagerSingletonInstance = this;
    }
}

void APhotonChatManager::BeginDestroy()
{
    Super::BeginDestroy();
    ResetInstance();
}

APhotonChatManager* APhotonChatManager::GetInstance(UWorld* World)
{
    if (!PhotonChatManagerSingletonInstance)
    {
        for (TActorIterator<APhotonChatManager> It(World); It; ++It)
        {
            PhotonChatManagerSingletonInstance = *It;
            break;
        }
        if (!PhotonChatManagerSingletonInstance)
        {
            PhotonChatManagerSingletonInstance = World->SpawnActor<APhotonChatManager>();
        }
    }
    return PhotonChatManagerSingletonInstance;
}

void APhotonChatManager::ResetInstance()
{
    PhotonChatManagerSingletonInstance = nullptr;
}

*/

void APhotonChatManager::SubscribeToChannel()
{
    if (chatClient)
    {
        if (chatClient)
        {
            ExitGames::Common::JString channels[] = { L"Global" };
            chatClient->opSubscribe(ExitGames::Common::JVector<ExitGames::Common::JString>(channels, sizeof(channels) / sizeof(ExitGames::Common::JString)));
            //UE_LOG(LogTemp, Warning, TEXT("Subscribed to channel: Global"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("chatClient is null ptr during SubscribeToChannel"));
        }
    }
}

void APhotonChatManager::SetPhotonNickNameToPlayFabDisplayName(const FString& ChatDisplayName)
{
    if (chatClient)
    {
        ExitGames::Common::JString jNewNickname(*ChatDisplayName);
        //chatClient->Set
        //chatClient->getLocalUser().setName(jNewNickname);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("chatClient is null ptr in SetPhotonNickname"));
    }
}

void APhotonChatManager::SendPublicMessage(FString const &message)
{
    if (chatClient)
    {
        
        ExitGames::Common::JString jMessage(*message); // Convert FString to JString
        chatClient->opPublishMessage(L"Global", jMessage);
        //GetDisplayName();
    }
}

void APhotonChatManager::ReceivePublicMessage(const FString& sender, const FString& message)
{
    
    FString displayName;

    // Check if the sender's display name is in our map.
    if (PhotonUserIDToPlayFabDisplayNameMap.Contains(sender))
    {
        displayName = *PhotonUserIDToPlayFabDisplayNameMap.Find(sender);
        OnPublicMessageReceived.Broadcast(displayName, message);
        UE_LOG(LogTemp, Warning, TEXT("sender's display name is in our map"));
    }
    else
    {
        // If not, fetch the display name from PlayFab.
        GetDisplayName(sender, message);
        UE_LOG(LogTemp, Warning, TEXT("sender's display name NOT in map...setting now"));
    }
}

void APhotonChatManager::GetDisplayName(const FString& sender, const FString& message)
{
    PlayFab::ClientModels::FGetPlayerProfileRequest Request;
    Request.PlayFabId = sender; // Assume sender is the PlayFabID

    clientAPI->GetPlayerProfile(Request,
        PlayFab::UPlayFabClientAPI::FGetPlayerProfileDelegate::CreateUObject(this, &APhotonChatManager::OnGetDisplayNameSuccess),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &APhotonChatManager::OnGetDisplayNameError)
    );

    // Store sender and message for use in the success callback.
    PendingMessageSender = sender;
    PendingMessageContent = message;
    UE_LOG(LogTemp, Warning, TEXT("Storing pending sender key: %s"), *PendingMessageSender);
}

void APhotonChatManager::OnGetDisplayNameSuccess(const PlayFab::ClientModels::FGetPlayerProfileResult& Result)
{
    if (Result.PlayerProfile)
    {
        DisplayName = Result.PlayerProfile->DisplayName;

        // Store the display name in our map.
        PhotonUserIDToPlayFabDisplayNameMap.Add(PendingMessageSender, DisplayName);
        UE_LOG(LogTemp, Warning, TEXT("Key added to map: %s"), *PendingMessageSender);
        UE_LOG(LogTemp, Warning, TEXT("DisplayName added to map: %s"), *PendingMessageSender);

        // Broadcast the message now that we have the display name.
        OnPublicMessageReceived.Broadcast(DisplayName, PendingMessageContent);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No player profile data received."));
    }
}

void APhotonChatManager::OnGetDisplayNameError(const PlayFab::FPlayFabCppError& ErrorResult) const
{
    // Handle error
    UE_LOG(LogTemp, Error, TEXT("Failed to get user data: %s"), *ErrorResult.ErrorMessage);
}


