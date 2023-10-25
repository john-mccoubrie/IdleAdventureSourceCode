
#include "AbilitySystem/Abilities/WoodcuttingAbility.h"
#include "Character/IdleCharacter.h"
#include "Player/IdlePlayerController.h"
#include "Actor/IdleActorManager.h"
#include "AbilitySystem/Abilities/WoodcuttingAbility.h"
#include "Actor/IdleEffectActor.h"
#include <Player/IdlePlayerState.h>
#include <AbilitySystemBlueprintLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <AbilitySystem/IdleAttributeSet.h>
#include <PlayerEquipment/BonusManager.h>
#include <Chat/GameChatManager.h>
#include <Test/TestManager.h>

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



    if (PC->CurrentTree->Tags.Contains("Legendary"))
    {
        bIsChoppingLegendaryTree = true;
        GetLegendaryEssence();
    }
    else
    {
        bIsChoppingLegendaryTree = false;
    }

    //FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
    //const FGameplayEffectSpecHandle EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(PC->WoodcuttingGameplayEffect, 1.f, EffectContextHandle);
    //ActiveEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
    PS->AbilitySystemComponent->OnPeriodicGameplayEffectExecuteDelegateOnSelf.RemoveAll(this);
    PS->AbilitySystemComponent->OnPeriodicGameplayEffectExecuteDelegateOnSelf.AddUObject(this, &UWoodcuttingAbility::CalculateLogYield);
    //PS->AbilitySystemComponent->OnPeriodicGameplayEffectExecuteDelegateOnTarget

    //int32 delegateCount = AbilitySystemComponent->OnPeriodicGameplayEffectExecuteDelegateOnTarget.GetAllocatedSize();
    //UE_LOG(LogTemp, Warning, TEXT("Number of Delegate Bindings: %d"), delegateCount);

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
    if (Character->EssenceCount <= 24)
    {
        Character->EssenceCount++;
        //UE_LOG(LogTemp, Warning, TEXT("Essence Added to inventory"));
        //UE_LOG(LogTemp, Warning, TEXT("EssenceCount: %f"), Character->EssenceCount);
        UItem* Essence = NewObject<UItem>();
        Character->CharacterInventory->AddItem(Essence);
    }
    else
    {
        AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
        GameChatManager->PostNotificationToUI("Inventory is full! Add your essence to the nearest coffer.");
    }
    
}

void UWoodcuttingAbility::CalculateLogYield(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecExecuted, FActiveGameplayEffectHandle ActiveHandle)
{
    //UE_LOG(LogTemp, Warning, TEXT("Periodic delegate called on target"));

    if (bIsChoppingLegendaryTree)
    {
        return; // Exit the function if chopping a legendary tree
    }

    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);
    ABonusManager* BonusManager = ABonusManager::GetInstance(GetWorld());

    //Woodcutting algorithm
    float LevelMultiplier = IdleAttributeSet->GetWoodcuttingLevel();
    //UE_LOG(LogTemp, Warning, TEXT("LevelMultiplier: %f"), LevelMultiplier);

    // Adjust the base chance and the level influence to balance the ease of gathering logs
    float BaseChance = 10.0f;
    float LevelInfluence = 0.5f;
    float ChanceToYield = BaseChance + (LevelMultiplier * LevelInfluence);
    //UE_LOG(LogTemp, Warning, TEXT("ChanceToYield: %f"), ChanceToYield);

    // Random factor to add some unpredictability
    float RandomFactor = FMath::RandRange(-10.0f, 10.0f);
    ChanceToYield += RandomFactor;
    ChanceToYield = FMath::Clamp(ChanceToYield, 0.0f, 100.0f);  // Ensure chance stays between 0 and 100
    //UE_LOG(LogTemp, Warning, TEXT("ChanceToYield after RandomFactor: %f"), ChanceToYield);

    float RandomRoll = FMath::RandRange(0, 100);
    //UE_LOG(LogTemp, Warning, TEXT("RandomRoll: %f"), RandomRoll);

    //uncomment this to return to normal algorithm
    RandomRoll = 1;
    ATestManager* TestManager = ATestManager::GetInstance(GetWorld());
    //if (RandomRoll <= ChanceToYield)
    if(TestManager->CurrentSettings.EssenceYieldSpeed <= ChanceToYield)
    {
        //UE_LOG(LogTemp, Warning, TEXT("EssenceYieldSpeed Value: %f"), TestManager->CurrentSettings.EssenceYieldSpeed);
        // Award the log
        //UE_LOG(LogTemp, Warning, TEXT("Get log in woodcuttingability!"));
        //AddEssenceToInventory();

        // Determine the type of log based on the rarity roll
        UItem* NewLog = NewObject<UItem>();
        float RarityRoll = FMath::RandRange(0.f, 100.f);

        //Default Essence to add multiper
        int EssenceToAdd = 1;
        if (RarityRoll <= 50.f)  // 50% chance for Wisdom
        {
            NewLog->EssenceRarity = "Wisdom";
            ExperienceGain = static_cast<float>(FMath::RoundToInt(10.0f * BonusManager->WisdomEssenceMultiplier));
            EssenceToAdd *= BonusManager->WisdomYieldMultiplier;
        }
        else if (RarityRoll <= 75.f)  // 25% chance for Temperance
        {
            NewLog->EssenceRarity = "Temperance";
            ExperienceGain = static_cast<float>(FMath::RoundToInt(20.0f * BonusManager->TemperanceEssenceMultiplier));
            EssenceToAdd *= BonusManager->TemperanceYieldMultiplier;
        }
        else if (RarityRoll <= 95.f)  // 20% chance for Justice
        {
            NewLog->EssenceRarity = "Justice";
            ExperienceGain = static_cast<float>(FMath::RoundToInt(30.0f * BonusManager->JusticeEssenceMultiplier));
            EssenceToAdd *= BonusManager->JusticeYieldMultiplier;
        }
        else  // 5% chance for Courage
        {
            NewLog->EssenceRarity = "Courage";
            ExperienceGain = static_cast<float>(FMath::RoundToInt(50.0f * BonusManager->CourageEssenceMultiplier));
            EssenceToAdd *= BonusManager->CourageYieldMultiplier;
        }

        AddExperience(ExperienceGain);

        // Load the data table
        UDataTable* EssenceDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Blueprints/UI/Inventory/DT_EssenceTypes.DT_EssenceTypes"));
        if (EssenceDataTable)
        {
            // Get the log data based on the essence rarity
            FEssenceData* EssenceData = EssenceDataTable->FindRow<FEssenceData>(NewLog->EssenceRarity, TEXT(""));
            if (EssenceData)
            {
                // Set the log properties from the data table
                NewLog->Name = EssenceData->Name;
                NewLog->Icon = EssenceData->Icon;
                NewLog->ItemDescription = EssenceData->Description;
            }
        }
        AIdleCharacter* Character = Cast<AIdleCharacter>(GetAvatarActorFromActorInfo());
        for (int i = 0; i < EssenceToAdd; ++i)
        {
            Character->CharacterInventory->AddItem(NewLog);
        }
    }
    //else
    //{
        //UE_LOG(LogTemp, Warning, TEXT("No log awarded"));
    //}
}

void UWoodcuttingAbility::AddExperience(float Amount)
{
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);

    //UE_LOG(LogTemp, Warning, TEXT("Before add: %f"), IdleAttributeSet->GetWoodcutExp());
    //UE_LOG(LogTemp, Warning, TEXT("Amount: %f"), Amount);
    IdleAttributeSet->SetWoodcutExp(IdleAttributeSet->GetWoodcutExp() + Amount);
    //UE_LOG(LogTemp, Warning, TEXT("After add: %f"), IdleAttributeSet->GetWoodcutExp());
}

void UWoodcuttingAbility::GetLegendaryEssence()
{
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    ABonusManager* BonusManager = ABonusManager::GetInstance(GetWorld());

    UItem* NewLog = NewObject<UItem>();
    int EssenceToAdd = 1;

    NewLog->EssenceRarity = "Legendary";

    //Test values (originally 1000)
    ATestManager* TestManager = ATestManager::GetInstance(GetWorld());
    float LegendaryExpGain = TestManager->CurrentSettings.LegendaryExpAmount;
    
    ExperienceGain = static_cast<float>(FMath::RoundToInt(LegendaryExpGain * BonusManager->LegendaryEssenceMultiplier));
    EssenceToAdd *= BonusManager->LegendaryYieldMultiplier;

    AddExperience(ExperienceGain);

    // Load the data table
    UDataTable* EssenceDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Blueprints/UI/Inventory/DT_EssenceTypes.DT_EssenceTypes"));
    if (EssenceDataTable)
    {
        // Get the log data based on the essence rarity
        FEssenceData* EssenceData = EssenceDataTable->FindRow<FEssenceData>(NewLog->EssenceRarity, TEXT(""));
        if (EssenceData)
        {
            // Set the log properties from the data table
            NewLog->Name = EssenceData->Name;
            NewLog->Icon = EssenceData->Icon;
            NewLog->ItemDescription = EssenceData->Description;
        }
    }
    AIdleCharacter* Character = Cast<AIdleCharacter>(GetAvatarActorFromActorInfo());
    for (int i = 0; i < EssenceToAdd; ++i)
    {
        Character->CharacterInventory->AddItem(NewLog);
    }

    UWorld* World = GetWorld();
    AIdleActorManager* IdleActorManager = AIdleActorManager::GetInstance(World);
    AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
    GameChatManager->PostNotificationToUI("You receive a legendary essence!");
    PC->CurrentTree->Tags.Remove("Legendary");
    IdleActorManager->SelectNewLegendaryTree();
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

