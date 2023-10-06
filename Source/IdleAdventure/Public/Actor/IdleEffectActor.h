#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "NiagaraComponent.h"
#include "AbilitySystemComponent.h"
#include "IdleEffectActor.generated.h"

class UGameplayEffect;
class USphereComponent;
class AbilitySystemComponent;
class UGameplayAbility;
class UWoodcuttingAbility;

UCLASS()
class IDLEADVENTURE_API AIdleEffectActor : public AActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USphereComponent* SphereComponent;


public:	
	AIdleEffectActor();

	UFUNCTION()
		void SetTreeLifespan(AIdleEffectActor* Tree);

	//float GetTotalDuration() const
	//{
		//return TotalDuration;
	//}

	//void SetTotalDuration(float newTotalDuration)
	//{
		//TotalDuration = newTotalDuration;
	//}


	UPROPERTY()
		UAbilitySystemComponent* AbilitySystemComponent;

		FTimerHandle RespawnTreeTimerHandle;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tree Respawn")
		float TotalDuration;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tree Respawn")
		float MinDuration;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tree Respawn")
		float MaxDuration;
		

		void ActivateLegendaryEffect();
		//Legendary Tree
		// Niagara system component for the legendary effect
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
		UNiagaraComponent* LegendaryEffectParticle;

		// Niagara system to be used for the legendary effect
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
		UNiagaraSystem* LegendaryEffect;


		//UGameplayEffect* WoodcuttingGameplayEffect;

	//UPROPERTY()
		//UWoodcuttingAbility* WoodcuttingAbility;

private:
	//float TotalDuration;
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TreeMeshComponent;
	
	
};
