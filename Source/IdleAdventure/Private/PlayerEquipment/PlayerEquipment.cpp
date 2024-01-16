
#include "PlayerEquipment/PlayerEquipment.h"
#include <Player/IdlePlayerState.h>
#include <Player/IdlePlayerController.h>
#include <AbilitySystem/IdleAttributeSet.h>
#include <PlayFab/PlayFabManager.h>
#include "Styling/SlateColor.h"
#include <Character/IdleCharacter.h>
#include <Kismet/GameplayStatics.h>
#include "PlayerEquipment/BonusManager.h"
#include <Chat/GameChatManager.h>

// Sets default values for this component's properties
UPlayerEquipment::UPlayerEquipment()
{
    PrimaryComponentTick.bCanEverTick = true;
}


void UPlayerEquipment::EquipItem(const FEquipmentData& ItemData)
{
    UE_LOG(LogTemp, Warning, TEXT("Entering EquipItem"));
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);
    AActor* Owner = GetOwner();

    USkeletalMeshComponent* CurrentlyEquippedMesh = nullptr;
    FString CurrentlyEquippedItemName = "";

    if (EquippedItemMeshes.Contains(ItemData.SocketName.ToString()))
    {
        CurrentlyEquippedMesh = EquippedItemMeshes[ItemData.SocketName.ToString()];
        CurrentlyEquippedItemName = EquippedItemNames[ItemData.SocketName.ToString()];
    }

    if (CurrentlyEquippedMesh)
    {
        if (CurrentlyEquippedItemName != ItemData.Name)
        {
            UE_LOG(LogTemp, Warning, TEXT("The socket name is the same"));

            FEquipmentData* OldItemDataPtr = GetEquipmentDataByName(CurrentlyEquippedItemName);
            if (!OldItemDataPtr)
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to fetch old item data for item: %s"), *CurrentlyEquippedItemName);
                return;
            }

            FEquipmentData OldItemData = *OldItemDataPtr;
            UnequipCurrentItem(OldItemData, CurrentlyEquippedMesh);

            ApplyEquipmentEffects(ItemData);

            //Play equip sound
            PC->IdleInteractionComponent->PlayEquipSound();
        }
        else
        {
            return;
        }
    }
    else
    {
        ApplyEquipmentEffects(ItemData);
        UE_LOG(LogTemp, Warning, TEXT("Equipped item is in a different socket than the item you're equipping"));
    }
    USkeletalMeshComponent* CharacterMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
    if (!Owner || !CharacterMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("Owner or CharacterMesh is not initialized!"));
        return;
    }

    // Create variables to hold whether the item exists and if the player level is high enough
    bool bItemExistsInTable = false;
    bool bPlayerLevelIsHighEnough = false;

    if (UEquipmentManager::Get().AllEquipmentDataTable)
    {
        static const FString ContextString(TEXT("General"));
        for (auto& Row : UEquipmentManager::Get().AllEquipmentDataTable->GetRowMap())
        {
            FEquipmentData* EquipmentData = (FEquipmentData*)Row.Value;
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
        AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
        GameChatManager->PostNotificationToUI(TEXT("Item not found in data table, cannot equip"), FLinearColor::Red);
        return;
    }

    if (!bPlayerLevelIsHighEnough)
    {
        UE_LOG(LogTemp, Warning, TEXT("Player level too low, cannot equip"));
        AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
        GameChatManager->PostNotificationToUI(TEXT("Player level too low, cannot equip"), FLinearColor::Red);
        return;
    }

    USkeletalMeshComponent* ItemMesh = NewObject<USkeletalMeshComponent>(Owner);
    ItemMesh->SetSkeletalMesh(ItemData.Mesh);
    ItemMesh->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, ItemData.SocketName);
    ItemMesh->RegisterComponent();

    EquippedItemMeshes.Add(ItemData.SocketName.ToString(), ItemMesh);
    EquippedItemMesh = ItemMesh;
    EquippedItemNames.Add(ItemData.SocketName.ToString(), ItemData.Name);

    UE_LOG(LogTemp, Warning, TEXT("Exiting EquipItem"));
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
    AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
    if (!GameChatManager) {
        UE_LOG(LogTemp, Error, TEXT("GameChatManager is null!"));
    }
    GameChatManager->PostNotificationToUI(TEXT("Player level too low, cannot equip"), FLinearColor::Red);
    return false;
}


void UPlayerEquipment::UnequipCurrentItem(const FEquipmentData& ItemData, USkeletalMeshComponent* MeshToDestroy)
{
    UE_LOG(LogTemp, Warning, TEXT("Entering UnequipItem for Item: %s"), *ItemData.Name);
    if (MeshToDestroy)
    {
        MeshToDestroy->DestroyComponent();
        RemoveEquipmentEffects(ItemData);
        UE_LOG(LogTemp, Warning, TEXT("Exiting UnequipItem for Item: %s"), *ItemData.Name);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("MeshToDestroy is null ptr"));
    }
}

void UPlayerEquipment::ApplyEquipmentEffects(const FEquipmentData& ItemData)
{
    UE_LOG(LogTemp, Error, TEXT("Applying Equipment Effects for Item: %s"), *ItemData.Name);
    ABonusManager* BonusManager = ABonusManager::GetInstance(GetWorld());
    BonusManager->ApplyEssenceBonus(ItemData.ItemBonus);
    UpdateItemName(ItemData);
}

void UPlayerEquipment::RemoveEquipmentEffects(const FEquipmentData& ItemData)
{
    UE_LOG(LogTemp, Error, TEXT("Removing Equipment Effects for Item: %s"), *ItemData.Name);
    ABonusManager* BonusManager = ABonusManager::GetInstance(GetWorld());
    BonusManager->RemoveEssenceBonus(ItemData.ItemBonus);
    UpdateItemName(ItemData);
}

FEquipmentData* UPlayerEquipment::GetEquipmentDataByName(const FString& ItemName)
{
    if (UEquipmentManager::Get().AllEquipmentDataTable)
    {
        static const FString ContextString(TEXT("General"));
        for (auto& Row : UEquipmentManager::Get().AllEquipmentDataTable->GetRowMap())
        {
            FEquipmentData* EquipmentData = (FEquipmentData*)Row.Value;
            if (EquipmentData->Name == ItemName)
            {
                return EquipmentData;
            }
        }
    }
    return nullptr;
}


bool UPlayerEquipment::PurchaseAndAddItemToPlayerEquipmentInventory(const FEquipmentData& ItemData)
{

    if (PlayerEquipmentInventory.Contains(ItemData.Name))
    {
        UE_LOG(LogTemp, Warning, TEXT("Player already owns the item in playerequipment class"));
        AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
        FString formattedMessage = FString::Printf(TEXT("You already own the %s"), *ItemData.Name);
        GameChatManager->PostNotificationToUI(formattedMessage, FLinearColor::Red);
        return false;
    }

    UE_LOG(LogTemp, Warning, TEXT("PurchaseAndAddItemToPlayerEquipmentInventory Reached"));
    if (!CanEquipItem(ItemData))
    {
        UE_LOG(LogTemp, Warning, TEXT("Player cannot equip the item"));
        return false;
    }

    APlayFabManager* PlayFabManager = APlayFabManager::GetInstance(GetWorld());
    if (!PlayFabManager)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayFabManager instance is null"));
        return false;
    }

    // Ensuring the player has enough essences before proceeding with the purchase
    if (!PlayFabManager->UpdatePlayFabEssenceCount(ItemData))
    {
        UE_LOG(LogTemp, Warning, TEXT("Insufficient essence to purchase the item"));
        AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
        FString formattedMessage = FString::Printf(TEXT("Insufficient essence to purchase %s"), *ItemData.Name);
        GameChatManager->PostNotificationToUI(formattedMessage, FLinearColor::Red);
        return false;
    }

    if (PlayFabManager->PurchaseEquipment(ItemData.Name, ItemData))
    {
        // Update the local inventory
        AddEquipmentItem(ItemData);

        AIdleCharacter* Character = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        if (Character)
        {
            // Then update the server-side data
            // Convert the TMap to TArray for passing to PlayFab manager
            TArray<FEquipmentData> NewPlayerEquipmentInventory;
            PlayerEquipmentInventory.GenerateValueArray(NewPlayerEquipmentInventory);
            PlayFabManager->UpdatePlayerEquipmentInventory(NewPlayerEquipmentInventory);

            AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
            FString formattedMessage = FString::Printf(TEXT("Successfully purchased the %s"), *ItemData.Name);
            GameChatManager->PostNotificationToUI(formattedMessage, FLinearColor::Green);

            PlayFabManager->OnPurchaseCompleted.Broadcast();

            UE_LOG(LogTemp, Warning, TEXT("Item added to Player Equipment Inventory"));

            //EquipItem(ItemData);
            return true;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Character is null"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PurchaseEquipment from playfabmanager failed in PlayerEquipment"));
    }

    return false;

}

void UPlayerEquipment::AddEquipmentItem(const FEquipmentData& ItemData)
{
    PlayerEquipmentInventory.Add(ItemData.Name, ItemData);
}

void UPlayerEquipment::UpdateItemName(const FEquipmentData& ItemData)
{
    if (ItemData.SocketName == "WeaponHandSocket")
    {
        ABonusManager* BonusManager = ABonusManager::GetInstance(GetWorld());
        BonusManager->UpdateStaffName(ItemData.Name);
    }
    else if (ItemData.SocketName == "CapeSocket")
    {
        ABonusManager* BonusManager = ABonusManager::GetInstance(GetWorld());
        BonusManager->UpdateCapeName(ItemData.Name);
    }
    else if (ItemData.SocketName == "AuraSocket")
    {
        ABonusManager* BonusManager = ABonusManager::GetInstance(GetWorld());
        BonusManager->UpdateAuraName(ItemData.Name);
    }
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