


#include "StoicStore/StoicStoreManager.h"
#include <Character/IdleCharacter.h>
#include "PlayerEquipment/PlayerEquipment.h"
#include <Kismet/GameplayStatics.h>
#include <Player/IdlePlayerController.h>
#include <Player/IdlePlayerState.h>

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
	//Instance of this might be causing it to not be called!
	UPlayerEquipment* PlayerEquipment = Cast<UPlayerEquipment>(Character->GetComponentByClass(UPlayerEquipment::StaticClass()));
	if (PlayerEquipment != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Returned true in stoicstoremanager"));
		//OnPurchaseCompleted.Broadcast(true);
		return PlayerEquipment->PurchaseAndAddItemToPlayerEquipmentInventory(ItemData);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("returned false in stoicstoremanager"));
		//OnPurchaseCompleted.Broadcast(false);
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

