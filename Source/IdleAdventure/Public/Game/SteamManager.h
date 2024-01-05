

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SteamManager.generated.h"

UCLASS()
class IDLEADVENTURE_API ASteamManager : public AActor
{
	GENERATED_BODY()
	
public:
	ASteamManager();
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	static ASteamManager* GetInstance(UWorld* World);
	void ResetInstance();

	bool IsSteamAchievementUnlocked(const FString& AchievementID);
	void UnlockSteamAchievement(const FString& AchievementID);
	FString GetSteamName();

private:
	static ASteamManager* SteamManagerSingletonInstance;

};
