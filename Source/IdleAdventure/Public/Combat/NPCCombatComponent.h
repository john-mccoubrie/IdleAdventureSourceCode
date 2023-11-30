

#pragma once

#include "CoreMinimal.h"
#include "Combat/BaseCombatComponent.h"
#include "NPCCombatComponent.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API UNPCCombatComponent : public UBaseCombatComponent
{
	GENERATED_BODY()
	
	virtual void HandleDeath() override;

};
