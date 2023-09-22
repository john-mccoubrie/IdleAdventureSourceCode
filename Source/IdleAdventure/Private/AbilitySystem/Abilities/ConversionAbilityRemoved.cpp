


#include "AbilitySystem/Abilities/ConversionAbilityRemoved.h"
#include <AbilitySystem/Abilities/ConversionAbility.h>

UConversionAbilityRemoved* UConversionAbilityRemoved::WaitForGameplayEffectRemoved(UGameplayAbility* OwningAbility, FActiveGameplayEffectHandle InHandle)
{
	UE_LOG(LogTemp, Warning, TEXT("WaitForGameplayEffectRemoved Reached"));
	UConversionAbilityRemoved* MyObj = NewAbilityTask<UConversionAbilityRemoved>(OwningAbility);
	return MyObj;
}

void UConversionAbilityRemoved::NotifyOnGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("NotifyOnGameplayEffectRemoved Called"));
	UConversionAbility* MyAbility = Cast<UConversionAbility>(Ability);
	if (MyAbility)
	{
		MyAbility->OnEffectRemoved.Broadcast(RemovalInfo);
		UE_LOG(LogTemp, Warning, TEXT("Removal info broadcast"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Conversion Ability is null"));
	}
}
