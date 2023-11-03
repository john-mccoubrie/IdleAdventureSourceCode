

#pragma once

#include "CoreMinimal.h"
#include <Character/IdleCharacter.h>
#include "Quest/Quest.h"
#include "GameFramework/Actor.h"
#include "NPCActor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractedWithNPC);

class UCapsuleComponent;

UCLASS()
class IDLEADVENTURE_API ANPCActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANPCActor();

	void Interact();

	UFUNCTION(BlueprintCallable, Category = "Quests")
	void AssignQuest(UQuest* Quest, AIdleCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "Quests")
	void CompleteQuest(UQuest* Quest, AIdleCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "Quests")
	void AddAvailableQuests(UQuest* QuestToAdd);

	UPROPERTY(BlueprintReadWrite, Category = "Quests")
	TArray<UQuest*> AvailableQuests;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* NPCMeshComponent;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInteractedWithNPC OnInteractedWithNPC;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
