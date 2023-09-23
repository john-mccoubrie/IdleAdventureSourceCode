

#pragma once

#include "CoreMinimal.h"
#include "Game/IdleGameModeBase.h"
#include "IdleGameModeLogin.generated.h"

/**
 * 
 */
UCLASS()
class IDLEADVENTURE_API AIdleGameModeLogin : public AIdleGameModeBase
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;
	

	UFUNCTION()
	void LoadIdleForestLevel();
};
