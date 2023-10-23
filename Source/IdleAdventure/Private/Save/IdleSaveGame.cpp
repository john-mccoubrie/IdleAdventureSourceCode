


#include "Save/IdleSaveGame.h"
#include <Kismet/GameplayStatics.h>

void UIdleSaveGame::SaveGame(int32 WoodcuttingExp, int32 PlayerLevel) 
{
	UIdleSaveGame* SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::CreateSaveGameObject(UIdleSaveGame::StaticClass()));
	SaveGameInstance->WoodcuttingExp = WoodcuttingExp;
	SaveGameInstance->PlayerLevel = PlayerLevel; 
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, "WoodcuttingSaveSlot", 0);
	UE_LOG(LogTemp, Warning, TEXT("Saved WoodcuttingExp: %d, PlayerLevel: %d"), WoodcuttingExp, PlayerLevel);
}

void UIdleSaveGame::LoadGame(int32& WoodcuttingExp, int32& PlayerLevel) 
{
	UIdleSaveGame* LoadGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("WoodcuttingSaveSlot", 0));
	if (LoadGameInstance)
	{
		WoodcuttingExp = LoadGameInstance->WoodcuttingExp;
		PlayerLevel = LoadGameInstance->PlayerLevel; 
		UE_LOG(LogTemp, Warning, TEXT("Loaded WoodcuttingExp: %d, PlayerLevel: %d"), WoodcuttingExp, PlayerLevel);
	}
}
