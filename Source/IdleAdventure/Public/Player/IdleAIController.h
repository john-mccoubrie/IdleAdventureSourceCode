

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "IdleAIController.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API AIdleAIController : public AAIController
{
	GENERATED_BODY()
	

public:
	AIdleAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
};
