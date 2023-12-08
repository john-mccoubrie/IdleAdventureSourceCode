


#include "Game/IdleGameModeBase.h"
#include <IdleGameInstance.h>
#include "PlayFab/PlayFabManager.h"
#include <Character/Enemy_Goblin_Boss.h>
#include "EngineUtils.h"

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
    //SpawnEnemyBoss();
    //Spawn the Game Chat Manager (Message of the day)
    GameChatManagerInstance = GetWorld()->SpawnActor<AGameChatManager>(AGameChatManager::StaticClass());

    GameChatManagerInstance->GetMessageOfTheDay();
    GameChatManagerInstance->PostNotificationToUI(TEXT("Welcome to StoicScape!"), FLinearColor::White);

    LeaderboardManagerInstance = NewObject<ULeaderboardManager>();
    BeginUpdateLeaderboard();
    // Retrieve the game instance
     //PhotonChatManagerInstance = GetWorld()->SpawnActor<APhotonChatManager>();
    PhotonChatManagerInstance = GetWorld()->SpawnActor<APhotonChatManager>();

    IdleGameInstance = Cast<UIdleGameInstance>(GetGameInstance());
     if (IdleGameInstance)
     {
         //ExitGames::Common::JString TempLoginData = IdleGameInstance->StoredLoginData;
         //ConnectToChat(TempLoginData);
         ConnectToChat(IdleGameInstance->StoredPlayFabUserID, IdleGameInstance->StoredPhotonToken);
     }
     
     //BonusManagerInstance = NewObject<UBonusManager>();
     
     CombatManagerInstance = GetWorld()->SpawnActor<ACombatManager>(ACombatManager::StaticClass());

     //Spawn the SpawnManager instance
     SpawnManagerInstance = GetWorld()->SpawnActor<ASpawnManager>(ASpawnManager::StaticClass());

     //Spawn the TestManager instance
     TestManagerInstance = GetWorld()->SpawnActor<ATestManager>(ATestManager::StaticClass());

     //Spawn the dialogue manager instance
     DialogueManagerInstance = GetWorld()->SpawnActor<ADialogueManager>(ADialogueManager::StaticClass());

     //Spawn the QuestManager instance
     QuestManagerInstance = GetWorld()->SpawnActor<AQuestManager>(AQuestManager::StaticClass());

     //Spawn the stoic store instance
     StoicStoreManagerInstance = GetWorld()->SpawnActor<AStoicStoreManager>(AStoicStoreManager::StaticClass());
     //Spawn the Bonus manager instnace
     BonusManagerInstance = GetWorld()->SpawnActor<ABonusManager>(ABonusManager::StaticClass());
     
     // Spawn the Sound manager instance
     if (SoundManagerBlueprint)
     {
         SoundManagerInstance = GetWorld()->SpawnActor<ASoundManager>(SoundManagerBlueprint);
     }

     // Spawn the PlayFabManager singleton instance
     PlayFabManagerInstance = GetWorld()->SpawnActor<APlayFabManager>(APlayFabManager::StaticClass());
     //Spawn the CofferManager singleton instance
     CofferManagerInstance = GetWorld()->SpawnActor<ACofferManager>(ACofferManager::StaticClass());
     CofferManagerInstance->SetUpAllCoffers();

     
     //Load quest data
     //PlayFabManagerInstance->FetchCompletedQuestsData();
}

void AIdleGameModeBase::ConnectToChat(ExitGames::Common::JString& userID, ExitGames::Common::JString& photonToken)
{
    if (PhotonChatManagerInstance)
    {
        PhotonChatManagerInstance->ConnectToChat(userID, photonToken);
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

void AIdleGameModeBase::SpawnEnemyBoss()
{
    // Define the spawn parameters
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();

    // Define the location and rotation for the new actor
    FVector Location = FVector(-21030.0, -11887.897393, 1623.752156); // Change this to your desired location
    FRotator Rotation = FRotator(0.0f, 0.0f, 0.0f); // Change this to your desired rotation

    // Spawn the actor
    AEnemy_Goblin_Boss* SpawnedActor = GetWorld()->SpawnActor<AEnemy_Goblin_Boss>(AEnemy_Goblin_Boss::StaticClass(), Location, Rotation, SpawnParams);
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