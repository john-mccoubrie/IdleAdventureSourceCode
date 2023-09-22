
#include "AbilitySystem/Abilities/WoodcuttingAbility.h"
#include "Character/IdleCharacter.h"
#include "Player/IdlePlayerController.h"
#include "AbilitySystem/Abilities/WoodcuttingAbility.h"
#include "Actor/IdleEffectActor.h"
#include <Player/IdlePlayerState.h>
#include <AbilitySystemBlueprintLibrary.h>
#include <Kismet/GameplayStatics.h>

int32 UWoodcuttingAbility::InstanceCounter = 0;

//still a lot of instances will need to fix this at some point
UWoodcuttingAbility::UWoodcuttingAbility()
{
    
    InstanceCounter++;
    //UE_LOG(LogTemp, Warning, TEXT("UWoodcuttingAbility instances: %d"), InstanceCounter);
}
UWoodcuttingAbility::~UWoodcuttingAbility()
{
    // Destructor logic
    InstanceCounter--;
    //UE_LOG(LogTemp, Warning, TEXT("UWoodcuttingAbility instances: %d"), InstanceCounter);
}

void UWoodcuttingAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    //UE_LOG(LogTemp, Warning, TEXT("Activate ability wca"));
    OnTreeLifespanChanged.AddDynamic(this, &UWoodcuttingAbility::SetDuration);
    bIsTreeBeingChopped = false;
    if (bAbilityIsActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("WoodcuttingAbility is already active."));
        return;
    }
    bAbilityIsActive = true;
    AIdleCharacter* Character = Cast<AIdleCharacter>(ActorInfo->AvatarActor.Get());
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    //AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PS);

    //PC->OnPeriodFired.AddDynamic(this, &UWoodcuttingAbility::AddEssenceToInventory);
    UAnimMontage* AnimMontage = Character->WoodcutMontage;
    Character->PlayAnimMontage(AnimMontage);

    //Spawn particle effect from the character
    PC->SpawnTreeCutEffect();


    //FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
    //const FGameplayEffectSpecHandle EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(PC->WoodcuttingGameplayEffect, 1.f, EffectContextHandle);
    //ActiveEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
    PS->AbilitySystemComponent->OnPeriodicGameplayEffectExecuteDelegateOnSelf.RemoveAll(this);
    PS->AbilitySystemComponent->OnPeriodicGameplayEffectExecuteDelegateOnSelf.AddUObject(this, &UWoodcuttingAbility::CalculateLogYield);
    //PS->AbilitySystemComponent->OnPeriodicGameplayEffectExecuteDelegateOnTarget

    //int32 delegateCount = AbilitySystemComponent->OnPeriodicGameplayEffectExecuteDelegateOnTarget.GetAllocatedSize();
    //UE_LOG(LogTemp, Warning, TEXT("Number of Delegate Bindings: %d"), delegateCount);

    bIsTreeBeingChopped = true;

    PC->CurrentWoodcuttingAbilityInstance = this;
}

void UWoodcuttingAbility::OnTreeCutDown()
{
    //UE_LOG(LogTemp, Warning, TEXT("OnTreeCutDown"));
    AIdleCharacter* Character = Cast<AIdleCharacter>(GetAvatarActorFromActorInfo());
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    PS->AbilitySystemComponent->OnPeriodicGameplayEffectExecuteDelegateOnSelf.RemoveAll(this);
    PC->bIsChoppingTree = false;
    bAbilityIsActive = false;

    //Deactivate particle effect from the character
    PC->EndTreeCutEffect();
    
    PS->AbilitySystemComponent->RemoveActiveGameplayEffect(PC->WoodcuttingEffectHandle);
    UAnimMontage* AnimMontage = Character->WoodcutMontage;
    Character->StopAnimMontage(AnimMontage);
    //EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UWoodcuttingAbility::SetDuration(float TotalDuration)
{
    //UE_LOG(LogTemp, Warning, TEXT("SetDuration"));
    GetWorld()->GetTimerManager().ClearTimer(StopWoodcuttingTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(StopWoodcuttingTimerHandle, this, &UWoodcuttingAbility::OnTreeCutDown, TotalDuration, false);
    FTimerManager& TimerManager = GetWorld()->GetTimerManager();
    float TimeRemainingBefore = TimerManager.GetTimerRemaining(StopWoodcuttingTimerHandle);
    //UE_LOG(LogTemp, Warning, TEXT("Time at set: %f"), TimeRemainingBefore);
}

void UWoodcuttingAbility::AddEssenceToInventory()
{
    AIdleCharacter* Character = Cast<AIdleCharacter>(GetAvatarActorFromActorInfo());
    UItem* Essence = NewObject<UItem>();
    Character->CharacterInventory->AddItem(Essence);
    Character->EssenceCount++;
    //UE_LOG(LogTemp, Warning, TEXT("Essence Added to inventory"));
    //UE_LOG(LogTemp, Warning, TEXT("EssenceCount: %f"), Character->EssenceCount);
}

void UWoodcuttingAbility::CalculateLogYield(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecExecuted, FActiveGameplayEffectHandle ActiveHandle)
{
    //UE_LOG(LogTemp, Warning, TEXT("Periodic delegate called on target"));
    //Unbind the delegate once the tree is chopped down

    ChanceToYield = 10 + (5 - 1);

    RandomRoll = FMath::RandRange(0, 100);
    if (RandomRoll <= ChanceToYield)
    {
        //Award the log
        //UE_LOG(LogTemp, Warning, TEXT("Get log in woodcuttingability!"));
        AddEssenceToInventory();
    }
    else
    {
        //UE_LOG(LogTemp, Warning, TEXT("No log awarded"));
    }
}

FActiveGameplayEffectHandle UWoodcuttingAbility::GetActiveEffectHandle() const
{
    return ActiveEffectHandle;
}

void UWoodcuttingAbility::StopCutDownTimer()
{
    FTimerManager& TimerManager = GetWorld()->GetTimerManager();

    if (GetWorld()->GetTimerManager().IsTimerActive(StopWoodcuttingTimerHandle))
    {
        // Check the time remaining before clearing the timer
        float TimeRemainingBefore = TimerManager.GetTimerRemaining(StopWoodcuttingTimerHandle);
        //UE_LOG(LogTemp, Warning, TEXT("Time remaining before clear: %f"), TimeRemainingBefore);

        TimerManager.ClearTimer(StopWoodcuttingTimerHandle);

        // Check the time remaining after clearing the timer
        float TimeRemainingAfter = TimerManager.GetTimerRemaining(StopWoodcuttingTimerHandle);
        //UE_LOG(LogTemp, Warning, TEXT("Time remaining after clear: %f"), TimeRemainingAfter);
    }
    else
    {
        //UE_LOG(LogTemp, Warning, TEXT("Timer is not active"));
    }

}

