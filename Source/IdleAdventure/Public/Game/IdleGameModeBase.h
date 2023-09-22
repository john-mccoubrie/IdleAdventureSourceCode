

#pragma once

#include "CoreMinimal.h"
#include "Chat/PhotonChatManager.h"
#include <Leaderboard/LeaderboardManager.h>
#include "IdleGameInstance.h"
#include <Leaderboard/UpdatePlayerDisplayName.h>
#include "GameFramework/GameModeBase.h"
#include "IdleGameModeBase.generated.h"


//class APhotonChatManager;
//class ULeaderboardManager;
//class AIdleActorManager;
/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API AIdleGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AIdleGameModeBase();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void ConnectToChat(ExitGames::Common::JString& userID);
	void BeginUpdateLeaderboard();

	// Accessor to get the leaderboard manager.
	ULeaderboardManager* GetLeaderboardManager() const { return LeaderboardManagerInstance; }

	UPROPERTY(BlueprintReadOnly, Category = "Photon")
	APhotonChatManager* PhotonChatManagerInstance;
	UIdleGameInstance* IdleGameInstance;

	UPROPERTY(BlueprintReadOnly, Category = "PlayFab")
	ULeaderboardManager* LeaderboardManagerInstance;
	
};
