

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "IdleInputConfig.generated.h"

USTRUCT(BlueprintType)
struct FIdleInputAction
{
	GENERATED_BODY()

		UPROPERTY(EditDefaultsOnly)
		const class UInputAction* InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly)
		FGameplayTag InputTag = FGameplayTag();
};


UCLASS()
class IDLEADVENTURE_API UIdleInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:

	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = false) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<FIdleInputAction> AbilityInputActions;
};
