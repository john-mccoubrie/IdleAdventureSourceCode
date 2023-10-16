

#pragma once

#include "CoreMinimal.h"
#include <PlayFab.h>
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include "PhotonListener.h"
#include "PhotonChat.h"
#include "PhotonChatManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPublicMessageReceived, const FString&, Sender, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChatConnected);
//class PhotonListener;
/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API APhotonChatManager : public AActor
{
	GENERATED_BODY()
	
	APhotonChatManager();
	~APhotonChatManager();

	//PhotonCloudListener ChatListener;
	//ExitGames::Chat::Listener chatListener;
	ExitGames::Common::JString UserID = L"abbf3a31-ccd8-4c23-8704-3ec991bc5d0b";
	//ExitGames::Chat::Client* chatListener;
	//ExitGames::Common::JString& Username;
	//ExitGames::Common::JString& ServerName;
	//FString appID;
	//UPhotonChat ChatPhoton;
	// chatListener;
	//PhotonChatListener ChatClient;
	//PhotonChatListener ChatListener;
	//FString UserID;
	



	//ExitGames::Common::JString& userName, const ExitGames::Common::JString& userID, const ExitGames::Common::JString& serverName, const nByte serverType = ExitGames::LoadBalancing::ServerType::NAME_SERVER, const bool bTryUseDatagramEncryption = false

public:
	void InitializeUserID(FString initUserID);
	void ConnectToChat(ExitGames::Common::JString& userID, ExitGames::Common::JString& photonToken);
	virtual void Tick(float DeltaTime) override;
	//virtual void BeginPlay() override;
	//virtual void BeginDestroy() override;
	//static APhotonChatManager* GetInstance(UWorld* World);
	//void ResetInstance();
	void SubscribeToChannel();
	void SetPhotonNickNameToPlayFabDisplayName(const FString& ChatDisplayName);

	UFUNCTION(BlueprintCallable, Category = "Photon")
	void SendPublicMessage(FString const &message);

	UFUNCTION(BlueprintCallable, Category = "Photon")
	void ReceivePublicMessage(const FString& sender, const FString& message);

	UPROPERTY(BlueprintAssignable, Category = "Chat|Events")
	FOnPublicMessageReceived OnPublicMessageReceived;

	UPROPERTY(BlueprintAssignable, Category = "Photon Chat")
	FOnChatConnected OnChatConnected;

	//UFUNCTION(BlueprintCallable, Category = "Photon")
	void GetDisplayName(const FString& sender, const FString& message);
	//UFUNCTION(BlueprintCallable, Category = "Photon")
	void OnGetDisplayNameSuccess(const PlayFab::ClientModels::FGetPlayerProfileResult& Result);
	//UFUNCTION(BlueprintCallable, Category = "Photon")
	void OnGetDisplayNameError(const PlayFab::FPlayFabCppError& ErrorResult) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Photon Chat")
	FString DisplayName;

private:
	PhotonListener* chatListener;
	ExitGames::Chat::Client* chatClient;
	FTimerHandle ServiceChatClientTimerHandle;
	PlayFabClientPtr clientAPI = nullptr;
	TMap<FString, FString> PhotonUserIDToPlayFabDisplayNameMap;
	FString PendingMessageSender;
	FString PendingMessageContent;
	//static APhotonChatManager* PhotonChatManagerSingletonInstance;
	
};
