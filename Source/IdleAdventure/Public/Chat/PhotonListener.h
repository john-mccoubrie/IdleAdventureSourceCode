

#pragma once
#include "PhotonChat.h"
#include "CoreMinimal.h"


class APhotonChatManager;
/**
 * 
 */
class IDLEADVENTURE_API PhotonListener : public ExitGames::Chat::Listener
{
public:
	PhotonListener();
	~PhotonListener();

	// Inherited via Listener
	void debugReturn(int debugLevel, const ExitGames::Common::JString& string) override;
	void onStateChange(int state) override;
	void connectionErrorReturn(int errorCode) override;
	void clientErrorReturn(int errorCode) override;
	void warningReturn(int warningCode) override;
	void serverErrorReturn(int errorCode) override;
	void connectReturn(int errorCode, const ExitGames::Common::JString& errorString) override;
	void disconnectReturn(void) override;
	void subscribeReturn(const ExitGames::Common::JVector<ExitGames::Common::JString>& channels, const ExitGames::Common::JVector<bool>& results) override;
	void unsubscribeReturn(const ExitGames::Common::JVector<ExitGames::Common::JString>& channels) override;
	void onStatusUpdate(const ExitGames::Common::JString& user, int status, bool gotMessage, const ExitGames::Common::Object& message) override;
	void onGetMessages(const ExitGames::Common::JString& channelName, const ExitGames::Common::JVector<ExitGames::Common::JString>& senders, const ExitGames::Common::JVector<ExitGames::Common::Object>& messages) override;
	void onPrivateMessage(const ExitGames::Common::JString& sender, const ExitGames::Common::Object& message, const ExitGames::Common::JString& channelName) override;


	APhotonChatManager* Owner = nullptr;
};
