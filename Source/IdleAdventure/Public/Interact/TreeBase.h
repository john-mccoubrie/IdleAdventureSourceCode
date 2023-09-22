#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interact/TargetInterface.h"
#include "TreeBase.generated.h"

class UStaticMeshComponent;

UCLASS()
class IDLEADVENTURE_API ATreeBase : public AActor, public ITargetInterface
{
	GENERATED_BODY()
	
public:	
	ATreeBase();
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* TreeMesh;

};
