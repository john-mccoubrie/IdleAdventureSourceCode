

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Combat/BaseCombatComponent.h"
#include "GoblinBossCombatComponent.generated.h"

class AEnemyBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangedStoicType, FString, StoicType);

UCLASS()
class IDLEADVENTURE_API UGoblinBossCombatComponent : public UBaseCombatComponent
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;
	virtual void HandleDeath() override;
	virtual void TakeDamage(float amount, float level) override;
	void StopDamageCheckTimer();


	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnChangedStoicType OnChangedStoicType;

	void InitializeDamagingCircleTimer();
	virtual void StopCircleDamageCheckTimer() override;
	virtual void StartCircleDamageCheckTimer() override;

	//Tutorial respawn system --add this to base combat at some point
	void RespawnEnemy();
	bool IsTutorialMap();
	UPROPERTY(EditDefaultsOnly, Category = "HealthPotion")
	TSubclassOf<AEnemyBase> TutorialGoblinBossBlueprint;
	FTimerHandle RespawnTimerHandle;
	FVector SavedLocation;
	FRotator SavedRotation;

private:
	FTimerHandle DamageCheckTimer;
	FTimerHandle ChangeStoicTypeTimer;
	FTimerHandle CircleSpawnTimerHandle;
	FTimerHandle CircleDamageCheckTimerHandle;
	UPROPERTY(EditAnywhere, Category = "Boss Combat")
	float CircleSpawnMin;
	UPROPERTY(EditAnywhere, Category = "Boss Combat")
	float CircleSpawnMax;
	UPROPERTY(EditAnywhere, Category = "Boss Combat")
	float CircleHitDamage;
	UPROPERTY(EditAnywhere, Category = "Boss Combat")
	float PendingDamage;
	UPROPERTY(EditAnywhere, Category = "Boss Combat")
	float DamageCircleRadius;
	UPROPERTY(EditAnywhere, Category = "Boss Combat")
	float TimeToDamage;
	FVector DamagingCircleCenter;
	FString CurrentStoicType;
	virtual void DamageCheck() override;
	FString BossChangeType();
	void ChangeStoicType();
	FString GetPlayerWeaponType();
	void SpawnDamagingCircle();
	void CheckForPlayerInCircle();
	void DestroyOwner();
	bool CanInitializeCircleTimer();

	bool bCanInitializeCircle = true;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* CircleDamageEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* ShardsDamageEffect;
};
