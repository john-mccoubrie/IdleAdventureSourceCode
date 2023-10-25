

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Coffer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCofferExpCount, float, OnCofferPercent);


class USphereComponent;
class UAbilitySystemComponent;

UCLASS()
class IDLEADVENTURE_API ACoffer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoffer();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USphereComponent* SphereComponent;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		UStaticMeshComponent* CofferMeshComponent;

	UPROPERTY()
		UAbilitySystemComponent* AbilitySystemComponent;

	void CofferClicked(FHitResult CofferHit);
	TMap<APlayerController*, int32> PlayersCofferUsage;

	//void StartExperienceTimer(float Duration);

	//FTimerHandle ExperienceTimerHandle;
	//float TotalExperienceTime;
	//float RemainingExperienceTime;

	//void DecrementExperienceTime();
	void AddExperienceTime(float AdditionalTime);

	UPROPERTY(BlueprintAssignable)
	FOnCofferExpCount OnCofferExpCount;

	ACoffer* ClickedCoffer;

	

private:
	TArray<APlayerController*> PlayersWhoUsedCoffer;

	void OnGameplayEffectFinished(APlayerController* PlayerController);

};
