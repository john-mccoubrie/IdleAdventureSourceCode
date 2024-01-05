

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "IdleSaveGame.generated.h"


class UAttributeSet;


USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	FString ItemID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 Row;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 Column;
};


UCLASS()
class IDLEADVENTURE_API UIdleSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, Category = "Default")
	int32 WoodcuttingExp;

	UPROPERTY(VisibleAnywhere, Category = "Default")
	int32 PlayerLevel;

	static void SaveGame(int32 WoodcuttingExp, int32 PlayerLevel);
	static void LoadGame(int32& WoodcuttingExp, int32& PlayerLevel);

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData", SaveGame)
	TMap<FString, FString> QuotesAndCategories;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData", SaveGame)
	TArray<FString> WisdomQuotes;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData", SaveGame)
	TArray<FString> TemperanceQuotes;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData", SaveGame)
	TArray<FString> JusticeQuotes;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData", SaveGame)
	TArray<FString> CourageQuotes;

	void AddQuoteAndSave(const FString& Message, const FString& Category);

	// Existing methods...
	void SaveQuotes(const TMap<FString, FString>& Quotes);
	TMap<FString, FString> LoadQuotes();

	void SaveDialogueStep(const int32 NewDialogueStep);
	int32 LoadDialogueStep();

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int32 DialogueStep;

	//UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	//FQuestProgress SavedQuestProgress;

	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void SetTutorialCompleted(bool Completed);

	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	bool IsTutorialCompleted() const;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData", SaveGame)
	bool bIsTutorialCompleted;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveGameData", SaveGame)
	TArray<FInventoryItem> SavedInventoryItems;

	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void SavePlayerEquipmentPosition(int32 Row, int32 Column);

	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	TArray<FInventoryItem> LoadPlayerEquipmentPosition();

};
