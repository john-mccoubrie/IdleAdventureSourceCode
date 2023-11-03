

#pragma once

#include "CoreMinimal.h"
#include "Chat/PhotonChatManager.h"
#include <Leaderboard/LeaderboardManager.h>
#include <Actor/CofferManager.h>
#include <Chat/GameChatManager.h>
#include <Quest/QuestManager.h>
#include <StoicStore/StoicStoreManager.h>
#include <Test/TestManager.h>
#include <Sound/SoundManager.h>
#include <PlayFab/PlayFabManager.h>
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
	void ConnectToChat(ExitGames::Common::JString& userID, ExitGames::Common::JString& photonToken);
	void BeginUpdateLeaderboard();

	// Accessor to get the leaderboard manager.
	ULeaderboardManager* GetLeaderboardManager() const { return LeaderboardManagerInstance; }

	UPROPERTY(BlueprintReadOnly, Category = "Photon")
	APhotonChatManager* PhotonChatManagerInstance;

	UIdleGameInstance* IdleGameInstance;

	UPROPERTY(BlueprintReadOnly, Category = "PlayFab")
	ULeaderboardManager* LeaderboardManagerInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StoicStore")
	AStoicStoreManager* StoicStoreManagerInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Manager")
	AGameChatManager* GameChatManagerInstance;

	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	ABonusManager* BonusManagerInstance;

	UPROPERTY(BlueprintReadOnly, Category = "Coffer Manager")
	ACofferManager* CofferManagerInstance;

	UPROPERTY(BlueprintReadOnly, Category = "Test Manager")
	ATestManager* TestManagerInstance;

	UPROPERTY(BlueprintReadOnly, Category = "Quest Manager")
	AQuestManager* QuestManagerInstance;

	UPROPERTY(BlueprintReadOnly, Category = "Sound")
	ASoundManager* SoundManagerInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	TSubclassOf<class ASoundManager> SoundManagerBlueprint;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayFabManager")
	//APlayFabManager* PlayFabManagerInstance;
	
};
