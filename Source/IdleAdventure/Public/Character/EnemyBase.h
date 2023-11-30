

#pragma once

#include "CoreMinimal.h"
#include "Combat/NPCCombatComponent.h"
#include "Actor/Base_NPCActor.h"
#include "AI/NPCAIController.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"


class UBehaviorTree;

UCLASS()
class IDLEADVENTURE_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

	virtual void Interact();
	virtual void EndCombatEffects();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UNPCCombatComponent* CombatComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TSubclassOf<UBehaviorTree> BehaviorTree;

	// Use TSubclassOf to specify the AI Controller class
	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<ANPCAIController> NPCAIControllerClass;

	//create the interact functions that work with the player controller

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
