

#pragma once

#include "CoreMinimal.h"
#include "UpdatePlayerDisplayName.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API UUpdatePlayerDisplayName : public UObject
{

	GENERATED_BODY()

public:
	UUpdatePlayerDisplayName();
	~UUpdatePlayerDisplayName();
	UFUNCTION(BlueprintCallable, Category = "PlayFab")
	void CustomUpdateDisplayName(const FString& DesiredDisplayName);
	UFUNCTION(BlueprintCallable, Category = "PlayFab|Profile")
	void FetchPlayerProfile(const FString& PlayFabId);
	
	
	void OnUpdateDisplayNameSuccess(const PlayFab::ClientModels::FUpdateUserTitleDisplayNameResult& Result);
	void OnUpdateDisplayNameError(const PlayFab::FPlayFabCppError& Error);
	void OnGetPlayerProfileSuccess(const PlayFab::ClientModels::FGetPlayerProfileResult& Result);
	void OnGetPlayerProfileError(const PlayFab::FPlayFabCppError& Error);

private:
	PlayFabClientPtr clientAPI = nullptr;
};
