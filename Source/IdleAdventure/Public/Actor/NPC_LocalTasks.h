

#pragma once

#include "CoreMinimal.h"
#include "Actor/Base_NPCActor.h"
#include "NPC_LocalTasks.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API ANPC_LocalTasks : public ABase_NPCActor
{
	GENERATED_BODY()
	
public:

	virtual void Interact() override;
};
