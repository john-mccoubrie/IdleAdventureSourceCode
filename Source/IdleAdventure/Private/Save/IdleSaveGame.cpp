


#include "Save/IdleSaveGame.h"
#include <Kismet/GameplayStatics.h>
#include <Chat/GameChatManager.h>
#include <Game/SteamManager.h>

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
	// Load existing quotes
	LoadExistingQuotes();

	// Check if the quote already exists
	if ((Category == "Wisdom" && WisdomQuotes.Contains(Message)) ||
		(Category == "Temperance" && TemperanceQuotes.Contains(Message)) ||
		(Category == "Justice" && JusticeQuotes.Contains(Message)) ||
		(Category == "Courage" && CourageQuotes.Contains(Message)))
	{
		// Log a message to the player
		UE_LOG(LogTemp, Warning, TEXT("The quote already exists in the category %s."), *Category);

		AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
		// Format the message using FString::Printf
		FString NotificationMessage = FString::Printf(TEXT("You already unlocked this quote!"));

		// Define the light red color
		FLinearColor LightRed = FLinearColor(1.0f, 0.5f, 0.5f, 1.0f);
		// Convert FLinearColor to FSlateColor
		FSlateColor NotificationColor = FSlateColor(LightRed);
		// Call the function with the formatted message and color
		GameChatManager->PostNotificationToUI(NotificationMessage, NotificationColor);
	}
	else
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

		AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
		GameChatManager->OnPostQuote.Broadcast(Message, Category);
		GameChatManager->PostNotificationToUI(TEXT("You unlocked a new Stoic quote! Check your meditations book to see the quotes you've collected."), FLinearColor::Yellow);
		// Save the updated quotes
		UGameplayStatics::SaveGameToSlot(this, "QuotesSaveSlot", 0);
	}
}

void UIdleSaveGame::SaveQuotes(const TMap<FString, FString>& NewQuotes)
{
	// Load existing save game instance or create a new one
	UIdleSaveGame* SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("QuotesSaveSlot", 0));
	if (!SaveGameInstance)
	{
		SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::CreateSaveGameObject(UIdleSaveGame::StaticClass()));
	}

	// Load existing quotes
	SaveGameInstance->LoadExistingQuotes();

	// Add new quotes to the existing ones
	for (const auto& Quote : NewQuotes)
	{
		if (!SaveGameInstance->WisdomQuotes.Contains(Quote.Key) &&
			!SaveGameInstance->TemperanceQuotes.Contains(Quote.Key) &&
			!SaveGameInstance->JusticeQuotes.Contains(Quote.Key) &&
			!SaveGameInstance->CourageQuotes.Contains(Quote.Key))
		{
			SaveGameInstance->QuotesAndCategories.Add(Quote.Key, Quote.Value);
		}
		else
		{
			// Log a message to the player
			UE_LOG(LogTemp, Warning, TEXT("The quote '%s' already exists."), *Quote.Key);
		}
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

void UIdleSaveGame::LoadExistingQuotes()
{
	UIdleSaveGame* LoadGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("QuotesSaveSlot", 0));
	if (LoadGameInstance)
	{
		WisdomQuotes = LoadGameInstance->WisdomQuotes;
		TemperanceQuotes = LoadGameInstance->TemperanceQuotes;
		JusticeQuotes = LoadGameInstance->JusticeQuotes;
		CourageQuotes = LoadGameInstance->CourageQuotes;
	}
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
	SaveCompletedLevel("Tutorial");
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

void UIdleSaveGame::SaveCompletedLevel(const FString& LevelName)
{
	// Load the existing save game or create a new one if it doesn't exist
	UIdleSaveGame* SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("CompletedLevelsSaveSlot", 0));
	if (!SaveGameInstance)
	{
		SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::CreateSaveGameObject(UIdleSaveGame::StaticClass()));
	}

	// Add the new level name to the array, avoiding duplicates
	if (!SaveGameInstance->CompletedLevels.Contains(LevelName))
	{
		SaveGameInstance->CompletedLevels.Add(LevelName);
	}

	// Save the updated save game object to the save slot
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, "CompletedLevelsSaveSlot", 0);
}

TArray<FString> UIdleSaveGame::LoadCompletedLevels()
{
	UIdleSaveGame* LoadGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("CompletedLevelsSaveSlot", 0));
	if (LoadGameInstance)
	{
		return LoadGameInstance->CompletedLevels;
	}
	return TArray<FString>();
}


void UIdleSaveGame::IncrementEssenceCount(int EssenceToAdd, UWorld* WorldContext)
{
	ASteamManager* SteamManager = ASteamManager::GetInstance(WorldContext);
	UIdleSaveGame* SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("TotalEssenceGatheredSaveSlot", 0));
	if (SaveGameInstance)
	{
		// Increment the total essence count by the amount added
		SaveGameInstance->TotalEssenceGathered += EssenceToAdd;
		// Save the updated total essence count
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, "TotalEssenceGatheredSaveSlot", 0);

		if (SaveGameInstance->TotalEssenceGathered >= 100)
		{
			if (SteamManager)
			{
				SteamManager->UnlockSteamAchievement(TEXT("ADEPT_PHILOSOPHER"));
			}
		}

		if (SaveGameInstance->TotalEssenceGathered >= 1000)
		{
			if (SteamManager)
			{
				SteamManager->UnlockSteamAchievement(TEXT("WISE_PHILOSOPHER"));
			}
		}
		
		if (SaveGameInstance->TotalEssenceGathered >= 10000)
		{
			if (SteamManager)
			{
				SteamManager->UnlockSteamAchievement(TEXT("BASED_PHILOSOPHER"));
			}
		}
		

		UE_LOG(LogTemp, Warning, TEXT("Current count: %i"), SaveGameInstance->TotalEssenceGathered);
		
	}
	else
	{
		// Initialize a new save game instance if none exists
		SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::CreateSaveGameObject(UIdleSaveGame::StaticClass()));
		SaveGameInstance->TotalEssenceGathered = EssenceToAdd;
		// Save the new essence count
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, "TotalEssenceGatheredSaveSlot", 0);
	}
}



