

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include <Actor/NPC_QuestGiver.h>
#include <Actor/HealthPotion.h>
#include "Actor/IdleInteractionComponent.h"
#include "AbilitySystemInterface.h"
#include <Character/EnemyBase.h>
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPeriodFiredDelegate);



USTRUCT(BlueprintType)
struct FPlayerControllerDefaults : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
	float WoodcuttingCastingDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
	float CofferCastingDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
	float NPCTalkingDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
	float EnemyFightingDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
	float ZMultiplierStaffEndLoc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
	float XMultiplierStaffEndLoc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
	float YMultiplierStaffEndLoc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
	float ZMultiplierTreeLoc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
	float MinZoomDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
	float MaxZoomDistance;

	FPlayerControllerDefaults()
		: WoodcuttingCastingDistance(0.0f)
		, CofferCastingDistance(0.0f)
		, NPCTalkingDistance(0.0f)
		, EnemyFightingDistance(0.0f)
		, ZMultiplierStaffEndLoc(0.0f)
		, XMultiplierStaffEndLoc(0.0f)
		, YMultiplierStaffEndLoc(0.0f)
		, ZMultiplierTreeLoc(0.0f)
		, MinZoomDistance(0.0f)
		, MaxZoomDistance(0.0f)
	{
	}
};

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	Idle,
	MovingToTree,
	MovingToCoffer,
	MovingToNPC,
	MovingToEnemy,
	MovingToObject,
	InCombat,
	CuttingTree
};

UCLASS()
class IDLEADVENTURE_API AIdlePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AIdlePlayerController();
	virtual void PlayerTick(float DeltaTime) override;
	void MoveTowardsTarget(AActor* Target, float CastingDistance, TFunction<void(APawn*)> OnReachTarget);

	UPROPERTY(BlueprintReadOnly, Category = "State")
	EPlayerState CurrentPlayerState = EPlayerState::Idle;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tree Interaction", meta = (AllowPrivateAccess = "true"))
	UIdleInteractionComponent* IdleInteractionComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

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
	float NPCTalkingDistance;

	UPROPERTY(EditAnywhere, Category = "Initial values")
	float EnemyFightingDistance;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Defaults DataTable")
	UDataTable* PlayerDefaultsTable;

	//Niagara system
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* MouseClickEffectSystem;
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* TreeClickEffectSystem;
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* CofferClickEffectSystem;
	UNiagaraComponent* MouseClickEffect;
	UNiagaraComponent* TreeClickEffect;
	UNiagaraComponent* CofferClickEffect;



	UFUNCTION()
	void WoodcuttingEXPEffect();

	AIdleEffectActor* CurrentTree = nullptr;

	void InterruptTreeCutting();
	void InteruptCombat();
	void StartAnimNotifyEnemyInteraction(APawn* PlayerPawn);
	//void FindEnemiesInRange(const FVector& Location, TArray<AEnemyBase*>& OutEnemies);

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	void InitializeActorManager();
    void SetupPlayerState();
    void SetupAbilitySystemComponent();
    void InitializeWoodcuttingAbility();
    void InitializeConversionAbility();
    void SetupInputSubsystem();
    void ConfigureMouseCursor();


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

	UPROPERTY(EditAnywhere, Category = "Zoom")
	float MinZoomDistance;

	UPROPERTY(EditAnywhere, Category = "Zoom")
	float MaxZoomDistance;


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
	FVector AdjustTargetZAxis(FVector NewTargetLocation);
	void ClickTree(FHitResult TreeHit, APawn* PlayerPawn);
	void OnCofferClicked(FHitResult CofferHit, APawn* PlayerPawn);
	void ResetWoodcuttingAbilityTimer();
	void StartWoodcuttingAbility(APawn* PlayerPawn);
	void StartConversionAbility(APawn* PlayerPawn);
	void StartNPCInteraction(APawn* PlayerPawn);
	void StartEnemyInteraction(APawn* PlayerPawn);
	void StartObjectInteraction(APawn* PlayerPawn);

	AActor* TargetTree = nullptr;
	AActor* TargetCoffer = nullptr;
	ABase_NPCActor* TargetNPC = nullptr;
	AEnemyBase* TargetEnemy = nullptr;
	AHealthPotion* TargetHealthPotion = nullptr;

	FHitResult CofferHitForCasting;



	FVector TargetLocation;


	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UIdleInputConfig> InputConfig;

	TObjectPtr<AActor> LastActor;
	TObjectPtr<AActor> ThisActor;
	//FHitResult CursorHit;
	FVector CachedDestination;

	UPROPERTY(EditAnywhere, Category = "Input")
	FVector TargetDestination;

	
	

};