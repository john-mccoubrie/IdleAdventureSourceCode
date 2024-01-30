
#include "Chat/GameChatManager.h"
#include "PlayFab.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include "EngineUtils.h"
#include <Save/IdleSaveGame.h>
#include <PhotonCloudAPIBPLibrary.h>

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

    
    LoadQuotes();

    //GetMessageOfTheDay();
    //PostNotificationToUI(TEXT("Welcome to StoicScape!"), FLinearColor::White);
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

void AGameChatManager::PostNotificationToUI(FString Message, FSlateColor Color)
{
    // Check if the delegate is bound to any function
    if (FOnPostGameNotification.IsBound())
    {
        FOnPostGameNotification.Broadcast(Message, Color);
        UE_LOG(LogTemp, Warning, TEXT("Notification posted to UI: %s"), *Message, Color);
    }
    else
    {
        // If not bound, delay the broadcasting by using a timer
        UE_LOG(LogTemp, Warning, TEXT("PostNotificationToUI not bound, delaying notification."));

        // Use a lambda to capture the 'Message' and call this function again
        FTimerDelegate TimerDel;
        TimerDel.BindLambda([this, Message, Color]()
            {
                PostNotificationToUI(Message, Color);
            });

        // Set the timer for a delay (e.g., 1 second)
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 1.0f, false);
    }
}

void AGameChatManager::PostNotificationToLoginScreen(FString Message, FSlateColor Color)
{
    UE_LOG(LogTemp, Warning, TEXT("OnPostGameNotificationToLoginScreen"));
    OnPostGameNotificationToLoginScreen.Broadcast(Message, Color);
}

void AGameChatManager::PostQuoteToMeditationsJournal(FString Message, FString Category)
{
    //PostNotificationToUI(TEXT("You unlocked a new Stoic quote! Check your meditations book to see the quotes you've collected."), FLinearColor::Yellow);
    //OnPostQuote.Broadcast(Message, Category);

    // Load existing quotes from save game or create a new one if it doesn't exist
    UIdleSaveGame* SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("QuotesSaveSlot", 0));
    if (!SaveGameInstance)
    {
        SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::CreateSaveGameObject(UIdleSaveGame::StaticClass()));
    }

    // Add the new quote to the appropriate category and save
    if (SaveGameInstance)
    {
        SaveGameInstance->AddQuoteAndSave(Message, Category);
        //OnPostQuote.Broadcast(Message, Category);
    }
}

void AGameChatManager::LoadQuotes()
{
    UIdleSaveGame* LoadGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("QuotesSaveSlot", 0));
    if (LoadGameInstance == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Save game instance not found."));
        return;
    }

    TWeakObjectPtr<AGameChatManager> WeakThis(this);

    // Ensure the delegate is bound
    if (!OnLoadSavedQuotes.IsBound())
    {
        UE_LOG(LogTemp, Error, TEXT("OnLoadSavedQuotes delegate is not bound."));
        return;
    }

    FTimerDelegate TimerDel;
    TimerDel.BindLambda([WeakThis, LoadGameInstance]()
        {
            if (!WeakThis.IsValid())
            {
                UE_LOG(LogTemp, Error, TEXT("GameChatManager is no longer valid."));
                return;
            }

            TArray<TArray<FString>*> Categories = { &LoadGameInstance->WisdomQuotes, &LoadGameInstance->TemperanceQuotes, &LoadGameInstance->JusticeQuotes, &LoadGameInstance->CourageQuotes };
            TArray<FString> CategoryNames = { "Wisdom", "Temperance", "Justice", "Courage" };

            for (int i = 0; i < Categories.Num(); ++i)
            {
                for (const FString& Quote : *Categories[i])
                {
                    if (!WeakThis.IsValid())
                    {
                        UE_LOG(LogTemp, Error, TEXT("GameChatManager is no longer valid during broadcasting."));
                        return;
                    }
                    WeakThis->OnLoadSavedQuotes.Broadcast(CategoryNames[i], Quote);
                    UE_LOG(LogTemp, Warning, TEXT("%s: %s"), *CategoryNames[i], *Quote);
                }
            }

            // Clear the timer as it's no longer needed
            WeakThis->GetWorld()->GetTimerManager().ClearTimer(WeakThis->LoadQuotesTimerHandle);
        });

    GetWorld()->GetTimerManager().SetTimer(LoadQuotesTimerHandle, TimerDel, 1.0f, true);
}





