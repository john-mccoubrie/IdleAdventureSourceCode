

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/Abilities/WoodcuttingAbility.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "IdlePlayerController.generated.h"


class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class ITargetInterface;
class UIdleInputConfig;
class IdleCharacter;
class UAttributeSet;
class UAbilitySystemComponent;
class UGameplayEffect;
class USplineComponent;
class ACoffer;
class AIdleActorManager;
class UNiagaraSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTreeClickedDelegate, AIdleEffectActor*, TreeClickedParam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPeriodFiredDelegate);


UCLASS()
class IDLEADVENTURE_API AIdlePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AIdlePlayerController();
	virtual void PlayerTick(float DeltaTime) override;


	UPROPERTY(EditAnywhere)
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintAssignable)
	FOnTreeClickedDelegate OnTreeClicked;

	UPROPERTY(BlueprintAssignable)
	FOnPeriodFiredDelegate OnPeriodFired;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> WoodcuttingGameplayEffect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> ConversionGameplayEffect;

	ACoffer* ClickedCoffer;

	FActiveGameplayEffectHandle WoodcuttingEffectHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gameplay Effects")
	UWoodcuttingAbility* CurrentWoodcuttingAbilityInstance = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* TreeCutEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* StaffEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USkeletalMesh* Staff;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USkeletalMeshComponent* Weapon;

	UPROPERTY(EditAnywhere, Category = "Input")
	float AdjustX;
	UPROPERTY(EditAnywhere, Category = "Input")
	float AdjustY;
	UPROPERTY(EditAnywhere, Category = "Input")
	float AdjustZ;

	UPROPERTY(EditAnywhere, Category = "Initial values")
	float WoodcuttingCastingDistance;

	UPROPERTY(EditAnywhere, Category = "Initial values")
	float CofferCastingDistance;

	UPROPERTY(EditAnywhere, Category = "Initial values")
	FVector StaffEndLocation;

	UPROPERTY(EditAnywhere, Category = "Initial values")
	float ZMultiplierStaffEndLoc;

	UPROPERTY(EditAnywhere, Category = "Initial values")
	float XMultiplierStaffEndLoc;

	UPROPERTY(EditAnywhere, Category = "Initial values")
	float YMultiplierStaffEndLoc;

	UPROPERTY(EditAnywhere, Category = "Initial values")
	float ZMultiplierTreeLoc;

	UPROPERTY(EditAnywhere, Category = "Initial values")
	float YawRotationStaffMultiplier;

	UPROPERTY(EditAnywhere, Category = "Initial values")
	float PitchRotationStaffMultiplier;

	UPROPERTY(EditAnywhere, Category = "Initial values")
	float RollRotationStaffMultiplier;

	void SpawnTreeCutEffect();
	void EndTreeCutEffect();
	UNiagaraComponent* SpawnedTreeEffect;
	UNiagaraComponent* SpawnedStaffEffect;


	UFUNCTION()
	void WoodcuttingEXPEffect();

	UPROPERTY(EditAnywhere, Category = "Input")
	bool bIsChoppingTree = false;

	AIdleEffectActor* CurrentTree = nullptr;


protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;


private:

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> IdleContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ClickAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ZoomAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> RotateHorizontalAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> RotateVerticalAction;

	UPROPERTY(EditDefaultsOnly, Category = "Zoom")
	float MinZoomDistance = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Zoom")
	float MaxZoomDistance = 1000.0f;


	float VerticalRotation = 0.0f; // Current vertical rotation in degrees
	float MaxVerticalRotation = 80.0f; // Maximum allowed vertical rotation in degrees
	float MinVerticalRotation = -80.0f; // Minimum allowed vertical rotation in degrees



	void Move(const FInputActionValue& InputActionValue);

	void RotateHorizontal(const FInputActionValue& InputActionValue);
	void RotateVertical(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void HandleClickAction(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void HandleZoomAction(const FInputActionValue& InputActionValue);

	void MoveToClickLocation(const FInputActionValue& InputActionValue, FHitResult CursorHit, APawn* PlayerPawn);
	void ClickTree(FHitResult TreeHit, APawn* PlayerPawn);
	void OnCofferClicked(FHitResult CofferHit, APawn* PlayerPawn);
	void ResetTreeTimer(AIdleEffectActor* Tree);
	void ResetWoodcuttingAbilityTimer();
	void StartWoodcuttingAbility(APawn* PlayerPawn);
	void StartConversionAbility(APawn* PlayerPawn);

	AActor* TargetTree = nullptr;
	AActor* TargetCoffer = nullptr;
	FHitResult CofferHitForCasting;
	bool bHasPerformedCofferClick;



	FVector TargetLocation;


	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UIdleInputConfig> InputConfig;

	TObjectPtr<AActor> LastActor;
	TObjectPtr<AActor> ThisActor;
	//FHitResult CursorHit;
	FVector CachedDestination;

	UPROPERTY(EditAnywhere, Category = "Input")
	bool bIsMovingToTarget = false;
	bool bIsMovingToCoffer = false;
	FVector TargetDestination;
	void InterruptTreeCutting();

};