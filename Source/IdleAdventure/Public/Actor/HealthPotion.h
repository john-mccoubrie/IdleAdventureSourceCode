
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPotion.generated.h"

UCLASS()
class IDLEADVENTURE_API AHealthPotion : public AActor
{
	GENERATED_BODY()
	
public:	

	AHealthPotion();
	void Interact();
	void ConsumeHealthPotion();

};
