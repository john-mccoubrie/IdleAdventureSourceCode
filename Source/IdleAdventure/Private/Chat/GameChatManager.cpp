
#include "Chat/GameChatManager.h"
#include "PlayFab.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include "EngineUtils.h"

AGameChatManager* AGameChatManager::GameChatManagerSingletonInstance = nullptr;

AGameChatManager::AGameChatManager()
{
    
}

void AGameChatManager::BeginPlay()
{
    Super::BeginPlay();
    if (!GameChatManagerSingletonInstance)
    {
        GameChatManagerSingletonInstance = this;
    }
    clientAPI = IPlayFabModuleInterface::Get().GetClientAPI();
    GetMessageOfTheDay();
    PostNotificationToUI(TEXT("Welcome to StoicScape!"));
}

void AGameChatManager::BeginDestroy()
{
    Super::BeginDestroy();
    ResetInstance();
}

AGameChatManager* AGameChatManager::GetInstance(UWorld* World)
{
    if (!GameChatManagerSingletonInstance)
    {
        for (TActorIterator<AGameChatManager> It(World); It; ++It)
        {
            GameChatManagerSingletonInstance = *It;
            break;
        }
        if (!GameChatManagerSingletonInstance)
        {
            GameChatManagerSingletonInstance = World->SpawnActor<AGameChatManager>();
        }
    }
    return GameChatManagerSingletonInstance;
}

void AGameChatManager::ResetInstance()
{
    GameChatManagerSingletonInstance = nullptr;
}

void AGameChatManager::GetMessageOfTheDay()
{
    PlayFab::ClientModels::FGetTitleDataRequest Request;
    Request.Keys.Add("MOTD");

    clientAPI->GetTitleData(Request,
        PlayFab::UPlayFabClientAPI::FGetTitleDataDelegate::CreateUObject(this, &AGameChatManager::OnGetMessageOfTheDaySuccess),
        PlayFab::FPlayFabErrorDelegate::CreateUObject(this, &AGameChatManager::OnGetMessageOfTheDayFailure)
    );
}

void AGameChatManager::OnGetMessageOfTheDaySuccess(const PlayFab::ClientModels::FGetTitleDataResult& Result)
{
    //Broadcast to UI
    // Check if the desired key is present in the result
    if (Result.Data.Contains(TEXT("MOTD")))
    {
        // Retrieve and use the message
        FString MessageOfTheDay = Result.Data[TEXT("MOTD")];
        UE_LOG(LogTemp, Warning, TEXT("Message of the Day: %s"), *MessageOfTheDay);
        PostMessageToUI(MessageOfTheDay);
        // Do something with MessageOfTheDay, e.g., display it in the chat
    }
    else
    {
        // Handle case where MOTD is not present
    }
}

void AGameChatManager::OnGetMessageOfTheDayFailure(const PlayFab::FPlayFabCppError& ErrorResult)
{
    UE_LOG(LogTemp, Error, TEXT("Failed to get message of the day: %s"), *ErrorResult.ErrorMessage);
}

void AGameChatManager::PostMessageToUI(FString Message)
{
    OnPostGameChat.Broadcast(Message);
    //UE_LOG(LogTemp, Warning, TEXT("MOTD broadcasted to UI"));
}

void AGameChatManager::PostNotificationToUI(FString Message)
{
    // Check if the delegate is bound to any function
    if (FOnPostGameNotification.IsBound())
    {
        FOnPostGameNotification.Broadcast(Message);
        UE_LOG(LogTemp, Warning, TEXT("Notification posted to UI: %s"), *Message);
    }
    else
    {
        // If not bound, delay the broadcasting by using a timer
        UE_LOG(LogTemp, Warning, TEXT("Delegate not bound, delaying notification."));

        // Use a lambda to capture the 'Message' and call this function again
        FTimerDelegate TimerDel;
        TimerDel.BindLambda([this, Message]()
            {
                PostNotificationToUI(Message);
            });

        // Set the timer for a delay (e.g., 1 second)
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 1.0f, false);
    }
}



