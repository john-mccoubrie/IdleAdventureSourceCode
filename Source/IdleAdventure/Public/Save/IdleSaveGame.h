

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "IdleSaveGame.generated.h"


class UAttributeSet;

/**
 * 
 */
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

	void SaveDialogueStep(const int32 NewDialogueStep);
	int32 LoadDialogueStep();

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int32 DialogueStep;

	//UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	//FQuestProgress SavedQuestProgress;
};
