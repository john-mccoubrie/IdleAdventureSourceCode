

#include "Chat/PhotonListener.h"
#include "PhotonChat.h"
//#include "CoreMinimal.h"
#include "Chat/PhotonChatManager.h"

PhotonListener::PhotonListener()
{
    //UE_LOG(LogTemp, Warning, TEXT("PhotonListener constructor called"));
}

PhotonListener::~PhotonListener()
{
}

void PhotonListener::debugReturn(int debugLevel, const ExitGames::Common::JString& string)
{
    //UE_LOG(LogTemp, Warning, TEXT("PhotonListener::debugReturn() - DebugLevel: %d, Message: %s"), debugLevel, *FString(string.UTF8Representation().cstr()));
}

void PhotonListener::onStateChange(int state)
{
    //UE_LOG(LogTemp, Warning, TEXT("PhotonListener::onStateChange() - State: %d"), state);
}

void PhotonListener::connectionErrorReturn(int errorCode)
{
    //UE_LOG(LogTemp, Warning, TEXT("PhotonListener::connectionErrorReturn() - ErrorCode: %d"), errorCode);
}

void PhotonListener::clientErrorReturn(int errorCode)
{
    UE_LOG(LogTemp, Warning, TEXT("PhotonListener::clientErrorReturn() - ErrorCode: %d"), errorCode);
}

void PhotonListener::warningReturn(int warningCode)
{
}

void PhotonListener::serverErrorReturn(int errorCode)
{
}

void PhotonListener::connectReturn(int errorCode, const ExitGames::Common::JString& errorString)
{
    if (errorCode == ExitGames::Chat::ErrorCode::OK)
    {
        UE_LOG(LogTemp, Warning, TEXT("Successfully connected and authenticated with Photon Chat!"));
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Successfully connected and authenticated with Photon Chat!"));

        if (Owner)
        {
            Owner->SubscribeToChannel();
            Owner->OnChatConnected.Broadcast(); // Call the delegate here
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Broadcast delegate to UI"));
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Owner is null"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to authenticate with Photon Chat. Error: %s"), *FString(errorString.UTF8Representation().cstr()));
    }
}

void PhotonListener::disconnectReturn(void)
{
}

void PhotonListener::subscribeReturn(const ExitGames::Common::JVector<ExitGames::Common::JString>& channels, const ExitGames::Common::JVector<bool>& results)
{
}

void PhotonListener::unsubscribeReturn(const ExitGames::Common::JVector<ExitGames::Common::JString>& channels)
{
}

void PhotonListener::onStatusUpdate(const ExitGames::Common::JString& user, int status, bool gotMessage, const ExitGames::Common::Object& message)
{
}

void PhotonListener::onGetMessages(const ExitGames::Common::JString& channelName, const ExitGames::Common::JVector<ExitGames::Common::JString>& senders, const ExitGames::Common::JVector<ExitGames::Common::Object>& messages)
{
    for (size_t i = 0; i < senders.getSize(); ++i)
    {
        FString sender = FString(senders[static_cast<unsigned int>(i)].UTF8Representation().cstr());
        FString message = FString(messages[static_cast<unsigned int>(i)].toString().UTF8Representation().cstr());

        Owner->ReceivePublicMessage(sender, message);
        //UE_LOG(LogTemp, Warning, TEXT("Received message in channel %s from %s: %s"), *FString(channelName.UTF8Representation().cstr()), *sender, *message);
        //FString onScreenMessage = FString::Printf(TEXT("Received message in chat! %s: %s"), *sender, *message);
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, onScreenMessage);
    }
}

void PhotonListener::onPrivateMessage(const ExitGames::Common::JString& sender, const ExitGames::Common::Object& message, const ExitGames::Common::JString& channelName)
{
}
