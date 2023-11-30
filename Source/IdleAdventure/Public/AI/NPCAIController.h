

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NPCAIController.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;

UCLASS()
class IDLEADVENTURE_API ANPCAIController : public AAIController
{
	GENERATED_BODY()
	
public:

	ANPCAIController();

protected:

	UPROPERTY()
	TObjectPtr<UBlackboardComponent> BlackboardComponent;

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

};
