

#pragma once

#include "CoreMinimal.h"
#include "Actor/Base_NPCActor.h"
#include "NPC_Conversational.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API ANPC_Conversational : public ABase_NPCActor
{
	GENERATED_BODY()
	
	virtual void Interact() override;

};
