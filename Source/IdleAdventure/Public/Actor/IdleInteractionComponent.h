

#pragma once

#include "CoreMinimal.h"
#include "IdleEffectActor.h"
#include "GameplayEffectTypes.h"
#include "Character/EnemyBase.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
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

	UFUNCTION(BlueprintCallable, Category = "Tree Interaction")
	void StartCombatAbility(APawn* PlayerPawn, AActor* Enemy);


	//Click actors
	AActor* TargetTree = nullptr;
	AActor* TargetCoffer = nullptr;
	AIdleEffectActor* CurrentTree = nullptr;


	//Begin animations
	void SpawnTreeCutEffect(APawn* PlayerPawn);
	void EndTreeCutEffect();
	void SpawnCombatEffect(APawn* PlayerPawn, AEnemyBase* TargetEnemy);
	void EndCombatEffect();

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void PlayStaffCastingSound();

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void StopStaffCastingSound();

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void PlayStaffDinkSound();

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void StopStaffDinkSound();

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void PlayCofferAddSound();

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void PlayInventoryFullSound();

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void PlayAddToLegendaryMeterSound();

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void PlayLegendaryTreeSpawnSound();

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void SpawnLevelUpEffect(APawn* PlayerPawn);
	UFUNCTION(BlueprintCallable, Category = "Sound")
	void StopLevelUpEffect();

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void PlayLevelUpSound();

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void PlayEquipSound();

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void PlayQuestReadyForTurnInSound();

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void PlayQuestTurnInSound();

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void PlayRunCompleteSound();

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void PlayPickupPotionSound();

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void PlayDialogueClickSound();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* EquipAudioComponent;
	UAudioComponent* WorldAudioComponent;

	void PlaySound(USoundBase* SoundToPlay);
	void PlayWorldSound(USoundBase* SoundToPlay);

	UNiagaraComponent* SpawnedTreeEffect;
	UNiagaraComponent* SpawnedStaffEffect;
	UNiagaraComponent* SpawnedAttackEffect;
	
	//Level up
	UNiagaraComponent* SpawnedLevelUpEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* LevelUpEffect;
	//Use the staff audio component
	UPROPERTY(EditAnywhere, Category = "Sound Properties")
	USoundCue* LevelUpSound;

	UPROPERTY(EditAnywhere, Category = "Sound Properties")
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, Category = "Sound Properties")
	USoundCue* PickupPotionSound;

	UPROPERTY(EditAnywhere, Category = "Sound Properties")
	USoundCue* DialogueClickSound;

	UPROPERTY(EditAnywhere, Category = "Sound Properties")
	USoundCue* QuestReadyForTurnInSound;

	UPROPERTY(EditAnywhere, Category = "Sound Properties")
	USoundCue* QuestTurnInSound;

	UPROPERTY(EditAnywhere, Category = "Sound Properties")
	USoundCue* RunCompleteSound;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* TreeCutEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* PlayerAttackEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* StaffEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* StaffAttackEffect;

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


	//Sound effects
	UPROPERTY(EditAnywhere, Category = "Sound Properties")
	USoundCue* StaffCastingSound;

	UPROPERTY(VisibleAnywhere, Category = "Sound Properties")
	UAudioComponent* StaffAudioComponent;

	UPROPERTY(EditAnywhere, Category = "Sound Properties")
	USoundCue* StaffDinkSound;

	UPROPERTY(EditAnywhere, Category = "Sound Properties")
	USoundCue* AddToCofferSound;

	UPROPERTY(EditAnywhere, Category = "Sound Properties")
	USoundCue* InventoryFullSound;

	UPROPERTY(EditAnywhere, Category = "Sound Properties")
	USoundCue* AddToLegendaryMeterSound;

	UPROPERTY(EditAnywhere, Category = "Sound Properties")
	USoundCue* LegendaryTreeSpawnSound;

	UPROPERTY(VisibleAnywhere, Category = "Sound Properties")
	UAudioComponent* StaffDinkAudioComponent;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
