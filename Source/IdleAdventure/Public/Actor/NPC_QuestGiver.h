

#pragma once

#include "CoreMinimal.h"
#include <Character/IdleCharacter.h>
#include "Quest/Quest.h"
#include "Base_NPCActor.h"
#include "GameFramework/Actor.h"
#include "NPC_QuestGiver.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractedWithNPC);

//class UCapsuleComponent;

UCLASS()
class IDLEADVENTURE_API ANPC_QuestGiver : public ABase_NPCActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANPC_QuestGiver();

	virtual void Interact() override;

	UFUNCTION(BlueprintCallable, Category = "Quests")
	void AssignQuest(UQuest* Quest, AIdleCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "Quests")
	void CompleteQuest(UQuest* Quest, AIdleCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "Quests")
	void AddAvailableQuests(UQuest* QuestToAdd);

	UFUNCTION()
	bool CanAcceptQuest(UQuest* Quest, AIdleCharacter* Player);


	UPROPERTY(BlueprintReadWrite, Category = "Quests")
	TArray<UQuest*> AvailableQuests;


	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInteractedWithNPC OnInteractedWithNPC;

	UFUNCTION()
	void HandleQuestVersionRetrieved(FString QuestID, FString QuestVersion);

};
