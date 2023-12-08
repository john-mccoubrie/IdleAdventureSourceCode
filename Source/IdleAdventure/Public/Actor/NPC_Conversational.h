

#pragma once

#include "CoreMinimal.h"
#include "Actor/Base_NPCActor.h"
#include "NPC_Conversational.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetDialogueStep, int32, DialogueOptionKey);

UCLASS()
class IDLEADVENTURE_API ANPC_Conversational : public ABase_NPCActor
{
	GENERATED_BODY()
	
	virtual void Interact() override;



	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FOnGetDialogueStep OnGetDialogueStep;
};
