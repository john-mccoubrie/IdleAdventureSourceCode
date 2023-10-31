

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "IdleAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

//custom delegates for more flexibility if I need to bind XP to quests, items, etc.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEXPChangedDelegate);

UCLASS()
class IDLEADVENTURE_API UIdleAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UIdleAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FOnEXPChangedDelegate OnEXPChanged;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WoodcutExp, Category = "Skills")
		FGameplayAttributeData WoodcutExp;
	ATTRIBUTE_ACCESSORS(UIdleAttributeSet, WoodcutExp);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WeeklyWoodcutExp, Category = "Skills")
	FGameplayAttributeData WeeklyWoodcutExp;
	ATTRIBUTE_ACCESSORS(UIdleAttributeSet, WeeklyWoodcutExp);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxWoodcutExp, Category = "Skills")
		FGameplayAttributeData MaxWoodcutExp;
	ATTRIBUTE_ACCESSORS(UIdleAttributeSet, MaxWoodcutExp);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WoodcuttingLevel, Category = "Skills")
		FGameplayAttributeData WoodcuttingLevel;
	ATTRIBUTE_ACCESSORS(UIdleAttributeSet, WoodcuttingLevel);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_XPToNextLevel, Category = "Skills")
		FGameplayAttributeData XPToNextLevel;
	ATTRIBUTE_ACCESSORS(UIdleAttributeSet, XPToNextLevel);


	UFUNCTION()
		void OnRep_WoodcutExp(const FGameplayAttributeData& OldWoodcutExp) const;

	UFUNCTION()
		void OnRep_WeeklyWoodcutExp(const FGameplayAttributeData& OldWeeklyWoodcutExp) const;

	UFUNCTION()
		void OnRep_MaxWoodcutExp(const FGameplayAttributeData& OldMaxWoodcutExp) const;

	UFUNCTION()
		void OnRep_WoodcuttingLevel(const FGameplayAttributeData& OldWoodcuttingLevel) const;
	UFUNCTION()
		void OnRep_XPToNextLevel(const FGameplayAttributeData& OldXPToNextLevel) const;

	UFUNCTION()
		void OnAttributeChange();

};
