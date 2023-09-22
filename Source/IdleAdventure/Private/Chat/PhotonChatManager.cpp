
#include "Chat/PhotonChatManager.h"
#include "Chat-cpp/inc/Listener.h"
#include <Chat/PhotonListener.h>


APhotonChatManager::APhotonChatManager()
{
    PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

APhotonChatManager::~APhotonChatManager()
{
    //delete chatListener;
    //delete chatClient;
}

void APhotonChatManager::InitializeUserID(FString initUserID)
{
    UserID = ExitGames::Common::JString(*initUserID); // convert Fstring to Jstring
    ConnectToChat(UserID);
}

void APhotonChatManager::ConnectToChat(ExitGames::Common::JString& userID)
{
    //UE_LOG(LogTemp, Warning, TEXT("ConnectToChat Called from PhotonChatManager"));

    chatListener = new PhotonListener();
    chatListener->Owner = this;

    static const ExitGames::Common::JString appID = L"abbf3a31-ccd8-4c23-8704-3ec991bc5d0b";
    static const ExitGames::Common::JString appVersion = L"1.0";
    chatClient = new ExitGames::Chat::Client(*chatListener, appID, appVersion);
    if (chatClient)
    {
        chatClient->setRegion(L"US");
        chatClient->connect(ExitGames::Chat::AuthenticationValues().setUserID(userID));
        //chatClient->service();
        //UE_LOG(LogTemp, Warning, TEXT("chatClient succesful in ConnectToChat"));
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

void APhotonChatManager::BeginDestroy()
{
    Super::BeginDestroy();
    //UE_LOG(LogTemp, Warning, TEXT("PhotonChatManager is being destroyed!"));
}

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

void APhotonChatManager::SendPublicMessage(FString const &message)
{
    if (chatClient)
    {
        ExitGames::Common::JString jMessage(*message); // Convert FString to JString
        chatClient->opPublishMessage(L"Global", jMessage);

    }
}

void APhotonChatManager::ReceivePublicMessage(const FString& sender, const FString& message)
{
    OnPublicMessageReceived.Broadcast(sender, message);
    //UE_LOG(LogTemp, Warning, TEXT("Messaged received in global channel"));
}


