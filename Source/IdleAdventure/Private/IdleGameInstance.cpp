


#include "IdleGameInstance.h"

UIdleGameInstance::UIdleGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//PhotonChatManager = ObjectInitializer.CreateDefaultSubobject<APhotonChatManager>(this, TEXT("PhotonChatManager"));
}

void UIdleGameInstance::Init()
{
    //UE_LOG(LogTemp, Warning, TEXT("IdleGameInstance Init called"));
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("IdleGameInstance Init called"));
    /*
    UWorld* CurrentWorld = GetWorld();
    if (CurrentWorld && CurrentWorld->IsGameWorld())
    {
        if (!PhotonChatManager)
        {
            PhotonChatManager = CurrentWorld->SpawnActor<APhotonChatManager>();
            if (PhotonChatManager)
            {
                UE_LOG(LogTemp, Warning, TEXT("PhotonManager created in Game Instance"));

                // Make the actor persistent across level transitions
                USceneComponent* RootComponent = PhotonChatManager->GetRootComponent();
                if (RootComponent)
                {
                    RootComponent->SetMobility(EComponentMobility::Movable);
                    PhotonChatManager->SetActorLocation(FVector::ZeroVector);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("PhotonChatManager does not have a valid root component!"));
                }

                // Assuming PersistentLevel is valid, add the actor
                if (CurrentWorld->PersistentLevel)
                {
                    CurrentWorld->PersistentLevel->AddLoadedActor(PhotonChatManager);
                    UE_LOG(LogTemp, Error, TEXT("Added to persistent level"));
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("PersistentLevel is not valid!"));
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn PhotonChatManager!"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Instance of photon chat manager already exists"));
        }
    }
    */
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


