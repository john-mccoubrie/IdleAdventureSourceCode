

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EquipmentActor.generated.h"

UCLASS()
class IDLEADVENTURE_API AEquipmentActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEquipmentActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Skeletal Mesh of the Equipment
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	USkeletalMeshComponent* EquipmentMesh;

	// Optional: Socket name where this equipment should be attached to the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FName AttachmentSocketName;

	

};
