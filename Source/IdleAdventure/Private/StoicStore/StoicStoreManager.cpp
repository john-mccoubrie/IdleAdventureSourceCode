


#include "StoicStore/StoicStoreManager.h"
#include <PlayerEquipment/PlayerEquipment.h>
#include <Character/IdleCharacter.h>
#include <Kismet/GameplayStatics.h>

// Sets default values
AStoicStoreManager::AStoicStoreManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

bool AStoicStoreManager::PurchaseItem(const FEquipmentData& ItemData)
{
	
	AIdleCharacter* Character = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	UPlayerEquipment* PlayerEquipment = Cast<UPlayerEquipment>(Character->GetComponentByClass(UPlayerEquipment::StaticClass()));
	return PlayerEquipment ? PlayerEquipment->PurchaseAndAddItemToPlayerEquipmentInventory(ItemData) : false;
	
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

