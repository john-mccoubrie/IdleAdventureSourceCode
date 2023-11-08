

#pragma once

#include "CoreMinimal.h"
#include <PlayFab.h>
#include "Styling/SlateColor.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include "GameFramework/Actor.h"
#include "GameChatManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPostGameChat, FString, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPostGameNotification, FString, Message, FSlateColor, Color);

UCLASS()
class IDLEADVENTURE_API AGameChatManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameChatManager();
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	static AGameChatManager* GetInstance(UWorld* World);
	void ResetInstance();
	void GetMessageOfTheDay();
	void OnGetMessageOfTheDaySuccess(const PlayFab::ClientModels::FGetTitleDataResult& Result);
	void OnGetMessageOfTheDayFailure(const PlayFab::FPlayFabCppError& ErrorResult);
	void PostMessageToUI(FString Message);
	void PostNotificationToUI(FString Message, FSlateColor Color);

	//Message from playfab
	UPROPERTY(BlueprintAssignable, Category = "Chat Events")
	FOnPostGameChat OnPostGameChat;

	//In game warnings
	UPROPERTY(BlueprintAssignable, Category = "Chat Events")
	FOnPostGameNotification FOnPostGameNotification;

private:
	static AGameChatManager* GameChatManagerSingletonInstance;
	PlayFabClientPtr clientAPI = nullptr;
};
