

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Base_NPCActor.generated.h"

class UCapsuleComponent;
class UBoxComponent;

UCLASS()
class IDLEADVENTURE_API ABase_NPCActor : public ACharacter
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABase_NPCActor();

	virtual void Interact();

	//UCapsuleComponent* GetMyCapsuleComponent() const { return GetCapsuleComponent(); }
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//UCapsuleComponent* CapsuleComponent;
	
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//UBoxComponent* CapsuleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* NPCMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* NPCIndicatorMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	float DesiredHeightAboveNPC = 50.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
