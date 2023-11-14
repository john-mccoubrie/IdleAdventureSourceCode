

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
	virtual void BeginDestroy() override;
	

	UFUNCTION()
	void LoadIdleForestLevel();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Manager")
	AGameChatManager* LoginGameChatManagerInstance;
};
