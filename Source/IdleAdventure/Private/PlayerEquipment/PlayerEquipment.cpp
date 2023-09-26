


#include "PlayerEquipment/PlayerEquipment.h"
#include <Player/IdlePlayerState.h>
#include <Player/IdlePlayerController.h>
#include <AbilitySystem/IdleAttributeSet.h>

// Sets default values for this component's properties
UPlayerEquipment::UPlayerEquipment()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


void UPlayerEquipment::EquipItem(const FEquipmentData& ItemData)
{
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);
    AActor* Owner = GetOwner();

    USkeletalMeshComponent* CharacterMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();

    // Create variables to hold whether the item exists and if the player level is high enough
    bool bItemExistsInTable = false;
    bool bPlayerLevelIsHighEnough = false;

    if (UEquipmentManager::Get().AllEquipmentDataTable)
    {
        static const FString ContextString(TEXT("General"));
        for (auto& Row : UEquipmentManager::Get().AllEquipmentDataTable->GetRowMap())
        {
            FEquipmentData* EquipmentData = (FEquipmentData*)Row.Value;
            UE_LOG(LogTemp, Warning, TEXT("Player Level: %f"), IdleAttributeSet->GetWoodcuttingLevel());
            if (EquipmentData->Name == ItemData.Name)
            {
                bItemExistsInTable = true;
                bPlayerLevelIsHighEnough = (IdleAttributeSet->GetWoodcuttingLevel() >= EquipmentData->LevelRequired);
                break;
            }
        }
    }

    if (!bItemExistsInTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("Item not found in data table, cannot equip"));
        return;
    }

    if (!bPlayerLevelIsHighEnough)
    {
        UE_LOG(LogTemp, Warning, TEXT("Player level too low, cannot equip"));
        return;
    }
    
    
     // Create a new Skeletal Mesh Component for the item
     USkeletalMeshComponent* ItemMesh = NewObject<USkeletalMeshComponent>(Owner);

     ItemMesh->SetSkeletalMesh(ItemData.Mesh);
     ItemMesh->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, ItemData.SocketName);
     ItemMesh->RegisterComponent();  // Register the component with the game engine

     // Store a reference to the currently equipped item mesh
     EquippedItemMesh = ItemMesh;

     // Apply any effects associated with the item
     ApplyEquipmentEffects(ItemData);       
}



bool UPlayerEquipment::CanEquipItem(const FEquipmentData& ItemData)
{
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);

    if (UEquipmentManager::Get().AllEquipmentDataTable)
    {
        static const FString ContextString(TEXT("General"));
        for (auto& Row : UEquipmentManager::Get().AllEquipmentDataTable->GetRowMap())
        {
            FEquipmentData* EquipmentData = (FEquipmentData*)Row.Value;
            if (EquipmentData->Name == ItemData.Name)
            {
                return (IdleAttributeSet->GetWoodcuttingLevel() >= EquipmentData->LevelRequired);
            }
        }
    }

    return false;
}


void UPlayerEquipment::UnequipCurrentItem()
{
	if (EquippedItemMesh)
	{
		EquippedItemMesh->DestroyComponent();
		EquippedItemMesh = nullptr;

		// Remove any effects associated with the previously equipped item
		RemoveEquipmentEffects();
	}
}

void UPlayerEquipment::ApplyEquipmentEffects(const FEquipmentData& ItemData)
{
	// TODO: Implement logic to apply effects (e.g., modify character stats)
}

void UPlayerEquipment::RemoveEquipmentEffects()
{
	// TODO: Implement logic to remove effects (e.g., reset character stats to their unequipped state)
}


// Called when the game starts
void UPlayerEquipment::BeginPlay()
{
	Super::BeginPlay();

	// ...
	//EquipDefaultItem();
}

void UPlayerEquipment::EquipDefaultItem(const FEquipmentData& ItemData)
{
    /*
	FEquipmentData DefaultEquipmentData;
	DefaultEquipmentData.Name = "Default Staff";
	DefaultEquipmentData.LevelRequired = 1;
	DefaultEquipmentData.Mesh = DefaultStaffMesh;  // Assuming you have a reference to the default staff mesh
	DefaultEquipmentData.SocketName = "WeaponHandSocket";  // Adjust this to match your skeleton's socket name
    */

    AActor* Owner = GetOwner();
    if (!Owner) return;

    USkeletalMeshComponent* CharacterMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();

    // Create a new Skeletal Mesh Component for the item
    USkeletalMeshComponent* ItemMesh = NewObject<USkeletalMeshComponent>(Owner);
    if (!ItemMesh) return;

    ItemMesh->SetSkeletalMesh(ItemData.Mesh);
    ItemMesh->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, ItemData.SocketName);
    ItemMesh->RegisterComponent();  // Register the component with the game engine

    // Store a reference to the currently equipped item mesh
    EquippedItemMesh = ItemMesh;
}


// Called every frame
void UPlayerEquipment::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

