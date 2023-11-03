

#pragma once

#include "CoreMinimal.h"
#include "IdleEffectActor.h"
#include "GameplayEffectTypes.h"
#include "NiagaraComponent.h"
#include "Components/ActorComponent.h"
#include "IdleInteractionComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTreeClickedDelegate, AIdleEffectActor*, TreeClickedParam);

class UNiagaraSystem;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IDLEADVENTURE_API UIdleInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UIdleInteractionComponent();

	UFUNCTION(BlueprintCallable, Category = "Tree Interaction")
	void ClickTree(FHitResult TreeHit, APawn* PlayerPawn);

	UFUNCTION(BlueprintCallable, Category = "Tree Interaction")
	void StartWoodcuttingAbility(APawn* PlayerPawn);



	//Click actors
	AActor* TargetTree = nullptr;
	AActor* TargetCoffer = nullptr;
	AIdleEffectActor* CurrentTree = nullptr;


	//Begin animations
	void SpawnTreeCutEffect(APawn* PlayerPawn);
	void EndTreeCutEffect();
	UNiagaraComponent* SpawnedTreeEffect;
	UNiagaraComponent* SpawnedStaffEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* TreeCutEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* StaffEffect;
	UPROPERTY(EditAnywhere, Category = "Initial values")
	FVector StaffEndLocation;
	UPROPERTY(EditAnywhere, Category = "Initial values")
	float ZMultiplierTreeLoc;

	UPROPERTY(EditAnywhere, Category = "Initial values")
	float YawRotationStaffMultiplier;

	UPROPERTY(EditAnywhere, Category = "Initial values")
	float PitchRotationStaffMultiplier;

	UPROPERTY(EditAnywhere, Category = "Initial values")
	float RollRotationStaffMultiplier;
	// End animations

	//Delegates
	UPROPERTY(BlueprintAssignable)
	FOnTreeClickedDelegate OnTreeClicked;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
