


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
     
     //BonusManagerInstance = NewObject<UBonusManager>();

     //Spawn the stoic store instance
     StoicStoreManagerInstance = GetWorld()->SpawnActor<AStoicStoreManager>(AStoicStoreManager::StaticClass());
     //Spawn the Bonus manager instnace
     BonusManagerInstance = GetWorld()->SpawnActor<ABonusManager>(ABonusManager::StaticClass());
     // Spawn the PlayFabManager singleton instance
     APlayFabManager* PlayFabManagerInstance = GetWorld()->SpawnActor<APlayFabManager>(APlayFabManager::StaticClass());
     //Spawn the CofferManager singleton instance
     CofferManagerInstance = GetWorld()->SpawnActor<ACofferManager>(ACofferManager::StaticClass());
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