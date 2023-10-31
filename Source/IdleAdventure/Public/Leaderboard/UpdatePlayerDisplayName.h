

#pragma once
#include "PlayFab.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include "CoreMinimal.h"
#include "UpdatePlayerDisplayName.generated.h"




/**
 * 
 */
UCLASS(Blueprintable)
class IDLEADVENTURE_API UUpdatePlayerDisplayName : public UObject
{

	GENERATED_BODY()

public:
	UUpdatePlayerDisplayName();
	~UUpdatePlayerDisplayName();
	UFUNCTION(BlueprintCallable, Category = "PlayFab")
	void UpdateDisplayName(const FString& DesiredDisplayName);
	UFUNCTION(BlueprintCallable, Category = "PlayFab|Profile")
	void FetchPlayerProfile(const FString& PlayFabId);
	
	
	void OnUpdateDisplayNameSuccess(const PlayFab::ClientModels::FUpdateUserTitleDisplayNameResult& Result);
	void OnUpdateDisplayNameError(const PlayFab::FPlayFabCppError& Error);
	void OnGetPlayerProfileSuccess(const PlayFab::ClientModels::FGetPlayerProfileResult& Result);
	void OnGetPlayerProfileError(const PlayFab::FPlayFabCppError& Error);

	bool bHasDisplayName = false;
	bool HasDisplayName() const { return bHasDisplayName; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateDisplayNameSuccess);
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnUpdateDisplayNameSuccess OnUpdateDisplayNameSuccessDelegate;

	//UIdleGameInstance* IdleGameInstance;

	FString TestDisplayName;

private:
	PlayFabClientPtr clientAPI = nullptr;
};
