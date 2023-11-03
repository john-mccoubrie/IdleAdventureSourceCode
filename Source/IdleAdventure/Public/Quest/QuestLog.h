

#pragma once

#include "CoreMinimal.h"
#include "Quest.h"
#include "UObject/NoExportTypes.h"
#include "QuestLog.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class IDLEADVENTURE_API UQuestLog : public UObject
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadWrite, Category = "Quests")
    TArray<UQuest*> Quests;

    UFUNCTION(BlueprintCallable, Category = "Quests")
    void AddQuest(UQuest* Quest);

    UFUNCTION(BlueprintCallable, Category = "Quests")
    void CompleteQuest(UQuest* Quest);
};
