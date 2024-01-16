

#pragma once

#include "CoreMinimal.h"
#include <PlayFab.h>
#include "Styling/SlateColor.h"
#include "Core/PlayFabClientDataModels.h"
#include "Core/PlayFabClientAPI.h"
#include "GameFramework/Actor.h"
#include "GameChatManager.generated.h"


USTRUCT(BlueprintType)
struct FQuoteCategoryPair
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Quote;

	UPROPERTY(BlueprintReadOnly)
	FString Category;

	FQuoteCategoryPair() {}
	FQuoteCategoryPair(const FString& InQuote, const FString& InCategory)
		: Quote(InQuote), Category(InCategory) {}
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPostGameChat, FString, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPostGameNotification, FString, Message, FSlateColor, Color);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPostGameNotificationToLoginScreen, FString, Message, FSlateColor, Color);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPostQuote, FString, Quote, FString, Category);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoadSavedQuotes, FString, Category, FString, Quote);

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
	void PostNotificationToLoginScreen(FString Message, FSlateColor Color);

	UFUNCTION(BlueprintCallable, Category = "Meditations Quotes")
	void PostQuoteToMeditationsJournal(FString Message, FString Category);

	void LoadQuotes();

	//bool DoesQuoteExist(const FString& Quote, const FString& Category);

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	TMap<FString, FString> QuotesAndCategories;

	//Message from playfab
	UPROPERTY(BlueprintAssignable, Category = "Chat Events")
	FOnPostGameChat OnPostGameChat;

	//In game warnings
	UPROPERTY(BlueprintAssignable, Category = "Chat Events")
	FOnPostGameNotification FOnPostGameNotification;

	UPROPERTY(BlueprintAssignable, Category = "Chat Events")
	FOnPostQuote OnPostQuote;

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnLoadSavedQuotes OnLoadSavedQuotes;
	
	//Login screen warnings
	UPROPERTY(BlueprintAssignable, Category = "Chat Events")
	FOnPostGameNotificationToLoginScreen OnPostGameNotificationToLoginScreen;

private:
	static AGameChatManager* GameChatManagerSingletonInstance;
	PlayFabClientPtr clientAPI = nullptr;
	FTimerHandle LoadQuotesTimerHandle;
};
