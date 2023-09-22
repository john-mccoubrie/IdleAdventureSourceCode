

#pragma once

#include "CoreMinimal.h"
#include "Chat/PhotonChatManager.h"
#include "Engine/GameInstance.h"
#include "IdleGameInstance.generated.h"


/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API UIdleGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UIdleGameInstance(const FObjectInitializer& ObjectInitializer);
	virtual void Init() override;
	//virtual void BeginDestroy() override;

	void ConnectToChat(ExitGames::Common::JString& userID);

	//UPROPERTY(BlueprintReadOnly, Category = "Photon")
	ExitGames::Common::JString StoredLoginData;

	/*
	UPROPERTY(BlueprintReadOnly, Category = "Photon")
	APhotonChatManager* PhotonChatManager;
	*/


private:
	
};
