


#include "IdleGameInstance.h"
#include <PlayerEquipment/EquipmentManager.h>
#include <PlayFab/PlayFabManager.h>

UIdleGameInstance::UIdleGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//PhotonChatManager = ObjectInitializer.CreateDefaultSubobject<APhotonChatManager>(this, TEXT("PhotonChatManager"));
}

void UIdleGameInstance::Init()
{
    UWorld* World = GetWorld();
    if (World)
    {
        FActorSpawnParameters SpawnParams;
        //SpawnParams.Owner = this;
        SpawnParams.Instigator = nullptr;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        // Set the location and rotation for the spawn
        FVector Location = FVector::ZeroVector;
        FRotator Rotation = FRotator::ZeroRotator;

        // Spawn the actor
        /*
        APlayFabManager* PlayFabManager = APlayFabManager::GetInstance(World);
        if (PlayFabManager)
        {
            UE_LOG(LogTemp, Log, TEXT("PlayFabManager spawned successfully"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn PlayFabManager"));
        }
        */
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("World is null in GameInstance Init"));
    }
}

/*
void UIdleGameInstance::BeginDestroy()
{
    //UE_LOG(LogTemp, Warning, TEXT("IdleGameInstance BeginDestroy called"));
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("IdleGameInstance BeginDestroy called"));
}
*/

void UIdleGameInstance::ConnectToChat(ExitGames::Common::JString& userID)
{
    
    /*
	//OnMapLoaded();
	if (PhotonChatManager)
	{
		PhotonChatManager->ConnectToChat(userID);
		UE_LOG(LogTemp, Warning, TEXT("Connect to chat called in IdleGameInstance"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("IdleGameInstance PhotonChatManager is null ptr"));
	}
    */
}


