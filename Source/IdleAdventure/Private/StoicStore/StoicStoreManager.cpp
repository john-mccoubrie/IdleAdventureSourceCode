


#include "StoicStore/StoicStoreManager.h"
#include <Character/IdleCharacter.h>
#include "PlayerEquipment/PlayerEquipment.h"
#include <Kismet/GameplayStatics.h>
#include <Player/IdlePlayerController.h>
#include "Styling/SlateColor.h"
#include <Player/IdlePlayerState.h>
#include <PlayFab/PlayFabManager.h>
#include <Chat/GameChatManager.h>

// Sets default values
AStoicStoreManager::AStoicStoreManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

bool AStoicStoreManager::PurchaseItem(const FEquipmentData& ItemData)
{
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    AIdleCharacter* Character = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    UPlayerEquipment* PlayerEquipment = Cast<UPlayerEquipment>(Character->GetComponentByClass(UPlayerEquipment::StaticClass()));

    if (PlayerEquipment != nullptr)
    {
        // Access the APlayFabManager instance and call PurchaseEquipment
        APlayFabManager* PlayFabManager = APlayFabManager::GetInstance(GetWorld()); // Assume GetInstance() provides access to the manager
        if (PlayFabManager)
        {
            if (PlayFabManager->PurchaseEquipment(ItemData.Name, ItemData) && PlayerEquipment->PurchaseAndAddItemToPlayerEquipmentInventory(ItemData))
            {
                // If purchase is successful, add item to player equipment inventory
                return true;
                UE_LOG(LogTemp, Warning, TEXT("Purchase successfull in stoic store manager"));
                AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
                GameChatManager->PostNotificationToUI(TEXT("You purchased this item."), FLinearColor::White);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Purchase failed in PlayFabManager"));
                return false;
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("PlayFabManager is null"));
            return false;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("returned false in stoicstoremanager"));
        return false;
    }	
}

// Called when the game starts or when spawned
void AStoicStoreManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AStoicStoreManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

