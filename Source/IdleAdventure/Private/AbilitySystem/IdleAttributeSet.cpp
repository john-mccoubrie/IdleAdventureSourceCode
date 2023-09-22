#include "AbilitySystem/IdleAttributeSet.h"
#include "Net/UnrealNetwork.h"

UIdleAttributeSet::UIdleAttributeSet()
{
    
	InitWoodcutExp(0.f);
	InitMaxWoodcutExp(100.f);
	InitWoodcuttingLevel(1.f);
    InitXPToNextLevel(100.f);
    
    //WoodcutExp.SetCurrentValue(0.f);
    //MaxWoodcutExp.SetCurrentValue(100.f);
    //WoodcuttingLevel.SetCurrentValue(1.f);
    //XPToNextLevel.SetCurrentValue(100.f);
}

void UIdleAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UIdleAttributeSet, WoodcutExp, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UIdleAttributeSet, MaxWoodcutExp, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UIdleAttributeSet, WoodcuttingLevel, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UIdleAttributeSet, XPToNextLevel, COND_None, REPNOTIFY_Always);
}

void UIdleAttributeSet::OnRep_WoodcutExp(const FGameplayAttributeData& OldWoodcutExp) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UIdleAttributeSet, WoodcutExp, OldWoodcutExp);
    OnEXPChanged.Broadcast();
}

void UIdleAttributeSet::OnRep_MaxWoodcutExp(const FGameplayAttributeData& OldMaxWoodcutExp) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UIdleAttributeSet, MaxWoodcutExp, OldMaxWoodcutExp);
}

void UIdleAttributeSet::OnRep_WoodcuttingLevel(const FGameplayAttributeData& OldWoodcuttingLevel) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UIdleAttributeSet, WoodcuttingLevel, OldWoodcuttingLevel);
}

void UIdleAttributeSet::OnRep_XPToNextLevel(const FGameplayAttributeData& OldXPToNextLevel) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UIdleAttributeSet, XPToNextLevel, OldXPToNextLevel);
}

void UIdleAttributeSet::OnAttributeChange()
{
    // Placeholder for replication logic if needed.
}
