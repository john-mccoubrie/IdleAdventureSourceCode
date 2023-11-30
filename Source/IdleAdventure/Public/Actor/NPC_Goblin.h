

#pragma once

#include "CoreMinimal.h"
#include "Combat/NPCCombatComponent.h"
#include "Actor/Base_NPCActor.h"
#include "AI/NPCAIController.h"
#include "NPC_Goblin.generated.h"

class UBehaviorTree;

UCLASS()
class IDLEADVENTURE_API ANPC_Goblin : public ABase_NPCActor
{
    GENERATED_BODY()

public:
    ANPC_Goblin();

    virtual void Interact() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    UNPCCombatComponent* CombatComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    TSubclassOf<UBehaviorTree> BehaviorTree;

    // Use TSubclassOf to specify the AI Controller class
    UPROPERTY(EditAnywhere, Category = "AI")
    TSubclassOf<ANPCAIController> NPCAIControllerClass;
};

