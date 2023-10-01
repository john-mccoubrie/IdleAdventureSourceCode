


#include "Game/IdleGameModeBase.h"
#include <IdleGameInstance.h>
#include "PlayFab/PlayFabManager.h"

//#include <Actor/IdleActorManager.h>

AIdleGameModeBase::AIdleGameModeBase()
{
    // Base properties
    PrimaryActorTick.bCanEverTick = true;

    

    // Initialization of other components or member variables
    //PhotonChatManagerInstance = NewObject<APhotonChatManager>();

    // Logging or other operations
    /*
    if (PhotonChatManagerInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("PhotonChatManagerInstance successfully created"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create PhotonChatManagerInstance"));
    }
    */
}

void AIdleGameModeBase::BeginPlay()
{
    LeaderboardManagerInstance = NewObject<ULeaderboardManager>();
    BeginUpdateLeaderboard();
    // Retrieve the game instance
     PhotonChatManagerInstance = GetWorld()->SpawnActor<APhotonChatManager>();
     IdleGameInstance = Cast<UIdleGameInstance>(GetGameInstance());
     if (IdleGameInstance)
     {
         ExitGames::Common::JString TempLoginData = IdleGameInstance->StoredLoginData;
         ConnectToChat(TempLoginData);
     }
     //USceneComponent* RootComponent = PhotonChatManagerInstance->GetRootComponent();
     //UE_LOG(LogTemp, Warning, TEXT("PhotonChatManager created on IdleGameMode begin play"));
     //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PhotonChatManager created on IdleGameMode begin play!"));

     //Spawn the stoic store instance
     StoicStoreManagerInstance = GetWorld()->SpawnActor<AStoicStoreManager>(AStoicStoreManager::StaticClass());
     //UE_LOG(LogTemp, Warning, TEXT("Store manager spawned"));

     // Spawn the PlayFabManager singleton instance
     APlayFabManager* PlayFabManagerInstance = GetWorld()->SpawnActor<APlayFabManager>(APlayFabManager::StaticClass());
     if (PlayFabManagerInstance)
     {
         UE_LOG(LogTemp, Log, TEXT("PlayFabManager instance spawned successfully"));
     }
     else
     {
         UE_LOG(LogTemp, Error, TEXT("Failed to spawn PlayFabManager instance"));
     }
}

void AIdleGameModeBase::ConnectToChat(ExitGames::Common::JString& userID)
{
    //OnMapLoaded();
    if (PhotonChatManagerInstance)
    {
        PhotonChatManagerInstance->ConnectToChat(userID);
        //UE_LOG(LogTemp, Warning, TEXT("Connect to chat called in IdleGameMode"));
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Connect to chat called in IdleGameMode"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("IdleGameMode PhotonChatManager is null ptr"));
    }
}

void AIdleGameModeBase::BeginUpdateLeaderboard()
{
    LeaderboardManagerInstance->Initialize(GetWorld());
}



void AIdleGameModeBase::Tick(float DeltaTime)
{
    /*
    Super::Tick(DeltaTime);
    
    if (PhotonChatManagerInstance)
    {
        //PhotonChatManagerInstance->CustomTick();
        UE_LOG(LogTemp, Warning, TEXT("Tick called in GameModeBase"));
    }
    else
    {
        //UE_LOG(LogTemp, Warning, TEXT("PhotonChatManagerInstance is nullptr"));
    }
    
    */
}

/*
ULeaderboardManager* AIdleGameModeBase::GetLeaderboardManager() const
{
    
    if (LeaderboardManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("Returning LeaderboardManager."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("LeaderboardManager is null!"));
    }

    return LeaderboardManager;
    
}
*/