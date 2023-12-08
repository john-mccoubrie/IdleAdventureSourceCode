

#pragma once

#include "CoreMinimal.h"
#include "Quest/Quest.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "DialogueManager.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGetMainStoryQuest, UQuest*, NewQuest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadMainStoryQuestProgress, FQuestProgress, LoadedQuestProgress);

USTRUCT(BlueprintType)
struct FDialogueEntry : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Dialogue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;

	// Add quest-related fields
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsQuestDialogue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsTurnInDialogue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString QuestID; // Identifier for the related quest

	FDialogueEntry()
		: Key(0), Name(""), Dialogue(""), Icon(nullptr),
		bIsQuestDialogue(false), bIsTurnInDialogue(false), QuestID("")
	{
	}
};

USTRUCT(BlueprintType)
struct FLocalQuest : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString QuestID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Version;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FQuestObjectives Objectives;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FQuestRewards Rewards;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FQuestProgress Progress;

	// Add quest-related fields
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bComplete;

	FLocalQuest()
		: QuestID("")
		, Version("")
		, Name("")
		, Category("")
		, Description("")
		, Objectives()
		, Rewards()
		, Progress()
		, bComplete(false)
	{
	}
};

UCLASS()
class IDLEADVENTURE_API ADialogueManager : public AActor
{
	GENERATED_BODY(Blueprintable)
	
public:	
	// Sets default values for this actor's properties
	ADialogueManager();
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	static ADialogueManager* GetInstance(UWorld* World);
	void ResetInstance();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	int32 GetCurrentDialogue();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SaveDialogueStep(int32 DialogueToSave);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool IsDialogueQuestStart(int32 DialogueStep);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	UQuest* GetQuestObjectivesForStory(const FString& QuestName);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	UQuest* CreateOrUpdateLocalStoryQuest(const FLocalQuest& LocalQuestData);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool IsMainStoryQuestReadyToTurnIn(UQuest* QuestToCheck);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void CompleteMainStoryQuest(UQuest* QuestToComplete);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SaveMainStoryQuestProgress(FQuestProgress ProgressToSave, UQuest* QuestToSave);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	FQuestProgress LoadMainStoryQuestProgress(FString QuestID);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void UpdateAndSaveQuestProgress(UQuest* Quest, const FString& ObjectiveType, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	FQuestProgress GetDefaultProgress();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool isQuestCompleteOnLoad(UQuest* QuestToCheck);

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FGetMainStoryQuest GetMainStoryQuest;

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnLoadMainStoryQuestProgress OnLoadMainStoryQuestProgress;

	//if it is a quest portion have a check at the beginning if the objectives are complete of getnext dialogue or something

	UPROPERTY()
	int32 SavedDialogueStep;

	UDataTable* DialogueDataTable;
	UDataTable* LocalQuestDataTable;
private:
	static ADialogueManager* DialogueManagerSingletonInstnace;
	bool isGameFirstLoad = true;
};
