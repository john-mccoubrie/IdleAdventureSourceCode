


#include "Save/IdleSaveGame.h"
#include <Kismet/GameplayStatics.h>

void UIdleSaveGame::SaveGame(int32 WoodcuttingExp, int32 PlayerLevel) 
{
	UIdleSaveGame* SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::CreateSaveGameObject(UIdleSaveGame::StaticClass()));
	SaveGameInstance->WoodcuttingExp = WoodcuttingExp;
	SaveGameInstance->PlayerLevel = PlayerLevel; 
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, "WoodcuttingSaveSlot", 0);
	//UE_LOG(LogTemp, Warning, TEXT("Saved WoodcuttingExp: %d, PlayerLevel: %d"), WoodcuttingExp, PlayerLevel);
}

void UIdleSaveGame::LoadGame(int32& WoodcuttingExp, int32& PlayerLevel) 
{
	UIdleSaveGame* LoadGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("WoodcuttingSaveSlot", 0));
	if (LoadGameInstance)
	{
		WoodcuttingExp = LoadGameInstance->WoodcuttingExp;
		PlayerLevel = LoadGameInstance->PlayerLevel; 
		UE_LOG(LogTemp, Warning, TEXT("Loaded WoodcuttingExp: %d, PlayerLevel: %d in Save game class"), WoodcuttingExp, PlayerLevel);
	}
}

void UIdleSaveGame::AddQuoteAndSave(const FString& Message, const FString& Category)
{
	// Add the new quote to the appropriate category
	if (Category == "Wisdom")
	{
		WisdomQuotes.Add(Message);
	}
	else if (Category == "Temperance")
	{
		TemperanceQuotes.Add(Message);
	}
	else if (Category == "Justice")
	{
		JusticeQuotes.Add(Message);
	}
	else if (Category == "Courage")
	{
		CourageQuotes.Add(Message);
	}

	// Save the updated quotes
	UGameplayStatics::SaveGameToSlot(this, "QuotesSaveSlot", 0);
}

void UIdleSaveGame::SaveQuotes(const TMap<FString, FString>& NewQuotes)
{
	// Load the existing saved game or create a new one if it doesn't exist
	UIdleSaveGame* SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("QuotesSaveSlot", 0));
	if (!SaveGameInstance)
	{
		SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::CreateSaveGameObject(UIdleSaveGame::StaticClass()));
	}

	// Add new quotes to the existing ones
	for (const auto& Quote : NewQuotes)
	{
		SaveGameInstance->QuotesAndCategories.Add(Quote.Key, Quote.Value);
	}

	// Save the updated list of quotes to the save slot
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, "QuotesSaveSlot", 0);
}

TMap<FString, FString> UIdleSaveGame::LoadQuotes()
{
	UIdleSaveGame* LoadGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("QuotesSaveSlot", 0));
	if (LoadGameInstance)
	{
		return LoadGameInstance->QuotesAndCategories;
	}
	return TMap<FString, FString>();
}

void UIdleSaveGame::SaveDialogueStep(const int32 NewDialogueStep)
{
	UIdleSaveGame* SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::CreateSaveGameObject(UIdleSaveGame::StaticClass()));
	SaveGameInstance->DialogueStep = NewDialogueStep;
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, "DialogueStepSaveSlot", 0);
}

int32 UIdleSaveGame::LoadDialogueStep()
{
	UIdleSaveGame* LoadGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("DialogueStepSaveSlot", 0));
	if (LoadGameInstance)
	{
		return LoadGameInstance->DialogueStep;
	}
	return 0;
}

void UIdleSaveGame::SetTutorialCompleted(bool Completed)
{
	bIsTutorialCompleted = Completed;
}

bool UIdleSaveGame::IsTutorialCompleted() const
{
	// Try to load an existing save game from the specified slot
	UIdleSaveGame* LoadedSaveGame = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("TutorialSaveSlot", 0));

	// Check if we successfully loaded a save game
	if (LoadedSaveGame)
	{
		// Return the value of bIsTutorialCompleted from the loaded save game
		return LoadedSaveGame->bIsTutorialCompleted;
	}
	else
	{
		// If there is no save game, return false or handle as needed
		return false;
	}
}

void UIdleSaveGame::SavePlayerEquipmentPosition(int32 Row, int32 Column)
{
	// Load the existing save game or create a new one if it doesn't exist
	UIdleSaveGame* SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("PlayerEquipmentPosition", 0));
	if (!SaveGameInstance)
	{
		SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::CreateSaveGameObject(UIdleSaveGame::StaticClass()));
	}

	// Create a new inventory item struct and set its properties
	FInventoryItem NewItem;
	NewItem.Row = Row;
	NewItem.Column = Column;

	// Add the new item to the saved inventory items array
	SaveGameInstance->SavedInventoryItems.Add(NewItem);

	// Save the updated save game object to the save slot
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, "PlayerEquipmentPosition", 0);
}

TArray<FInventoryItem> UIdleSaveGame::LoadPlayerEquipmentPosition()
{
	UIdleSaveGame* LoadGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("PlayerEquipmentPosition", 0));
	if (LoadGameInstance)
	{
		return LoadGameInstance->SavedInventoryItems;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No PlayerEquipmentPosition save slot"));
	}

	return TArray<FInventoryItem>();
}
