


#include "IdleGameInstance.h"
#include <PlayerEquipment/EquipmentManager.h>

UIdleGameInstance::UIdleGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//PhotonChatManager = ObjectInitializer.CreateDefaultSubobject<APhotonChatManager>(this, TEXT("PhotonChatManager"));
}

void UIdleGameInstance::Init()
{
		
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


