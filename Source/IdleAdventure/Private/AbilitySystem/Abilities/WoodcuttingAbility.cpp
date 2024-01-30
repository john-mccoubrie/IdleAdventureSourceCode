
#include "AbilitySystem/Abilities/WoodcuttingAbility.h"
#include "Character/IdleCharacter.h"
#include "Player/IdlePlayerController.h"
#include "Actor/IdleActorManager.h"
#include "Styling/SlateColor.h"
#include "AbilitySystem/Abilities/WoodcuttingAbility.h"
#include "Actor/IdleEffectActor.h"
#include <Player/IdlePlayerState.h>
#include <AbilitySystemBlueprintLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <AbilitySystem/IdleAttributeSet.h>
#include <PlayerEquipment/BonusManager.h>
#include <Chat/GameChatManager.h>
#include <Test/TestManager.h>
#include <Actor/CofferManager.h>
#include <Quest/DialogueManager.h>
#include <Game/SpawnManager.h>
#include <Save/IdleSaveGame.h>

int32 UWoodcuttingAbility::InstanceCounter = 0;

//still a lot of instances will need to fix this at some point
UWoodcuttingAbility::UWoodcuttingAbility()
{
    
    InstanceCounter++;
    WisdomThreshold = 50.0f;
    TemperanceThreshold = 75.0f;
    JusticeThreshold = 95.0f;
    CourageThreshold = 100.0f;
    UE_LOG(LogTemp, Warning, TEXT("UWoodcuttingAbility instances: %d"), InstanceCounter);
}
UWoodcuttingAbility::~UWoodcuttingAbility()
{
    // Destructor logic
    InstanceCounter--;
    UE_LOG(LogTemp, Warning, TEXT("UWoodcuttingAbility instances: %d"), InstanceCounter);
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
    //PC->SpawnTreeCutEffect();



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
    PC->CurrentPlayerState = EPlayerState::Idle;
    bAbilityIsActive = false;

    //Deactivate particle effect from the character
    PC->IdleInteractionComponent->EndTreeCutEffect();
    PC->IdleInteractionComponent->StopStaffCastingSound();
    PC->IdleInteractionComponent->PlayStaffDinkSound();
    
    PS->AbilitySystemComponent->RemoveActiveGameplayEffect(PC->WoodcuttingEffectHandle);
    UAnimMontage* AnimMontage = Character->WoodcutMontage;
    Character->StopAnimMontage(AnimMontage);
    
    ASpawnManager* SpawnManager = ASpawnManager::GetInstance(GetWorld());
    SpawnManager->UpdateTreeCount(1);

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
    //UE_LOG(LogTemp, Warning, TEXT("End ability"));
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
        GameChatManager->PostNotificationToUI(TEXT("Inventory is full! Add your essence to the nearest cart. Woodcutting ability"), FLinearColor::Red);
    }
    
}

void UWoodcuttingAbility::CalculateLogYield(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecExecuted, FActiveGameplayEffectHandle ActiveHandle)
{
    //UE_LOG(LogTemp, Warning, TEXT("Periodic delegate called on target"));
    

    if (bIsChoppingLegendaryTree)
    {
        return; // Exit the function if chopping a legendary tree
    }
    ATestManager* TestManager = ATestManager::GetInstance(GetWorld());
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    AIdleCharacter* Character = Cast<AIdleCharacter>(GetAvatarActorFromActorInfo());
    UIdleAttributeSet* IdleAttributeSet = CastChecked<UIdleAttributeSet>(PS->AttributeSet);
    ABonusManager* BonusManager = ABonusManager::GetInstance(GetWorld());

    if (Character->EssenceCount >= 24)
    {
        OnTreeCutDown();
        AIdleActorManager* IdleActorManager = AIdleActorManager::GetInstance(GetWorld());
        IdleActorManager->OnCountdownFinished(PC->CurrentTree);
        PC->InterruptTreeCutting();
        AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
        GameChatManager->PostNotificationToUI(TEXT("Inventory is full! Add your essence to the nearest cart."), FLinearColor::Red);
        PC->IdleInteractionComponent->PlayInventoryFullSound();
        return;
    }


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
    ChanceToYield *= TestManager->CurrentSettings.LogYieldFrequencyFactor;
    ChanceToYield = FMath::Clamp(ChanceToYield, 0.0f, 100.0f);  // Ensure chance stays between 0 and 100
    //UE_LOG(LogTemp, Warning, TEXT("ChanceToYield after RandomFactor: %f"), ChanceToYield);

    float RandomRoll = FMath::RandRange(0, 100);
    //UE_LOG(LogTemp, Warning, TEXT("RandomRoll: %f"), RandomRoll);

    //uncomment this to return to normal algorithm
    //RandomRoll = 1;
    
    //if (RandomRoll <= ChanceToYield)
    if(RandomRoll <= ChanceToYield)
    {
        //UE_LOG(LogTemp, Warning, TEXT("EssenceYieldSpeed Value: %f"), TestManager->CurrentSettings.EssenceYieldSpeed);
        // Award the log
        //UE_LOG(LogTemp, Warning, TEXT("Get log in woodcuttingability!"));
        //AddEssenceToInventory();

        // Determine the type of log based on the rarity roll
        UItem* NewLog = NewObject<UItem>();

        WisdomThreshold = 50.0f;
        TemperanceThreshold = 75.0f;
        JusticeThreshold = 95.0f;
        CourageThreshold = 100.0f;

        WisdomThreshold -= BonusManager->MultiplierSet.WisdomEssenceChanceMultiplier;
        TemperanceThreshold -= BonusManager->MultiplierSet.TemperanceEssenceChanceMultiplier;
        JusticeThreshold -= BonusManager->MultiplierSet.JusticeEssenceChanceMultiplier;
        CourageThreshold -= BonusManager->MultiplierSet.CourageEssenceChanceMultiplier;

        float RarityRoll = FMath::RandRange(0.f, 100.f);

        //UE_LOG(LogTemp, Warning, TEXT("WisdomThreshold: %f"), WisdomThreshold);
        //UE_LOG(LogTemp, Warning, TEXT("TemperanceThreshold: %f"), TemperanceThreshold);
        //UE_LOG(LogTemp, Warning, TEXT("JusticeThreshold: %f"), JusticeThreshold);
        //UE_LOG(LogTemp, Warning, TEXT("CourageThreshold: %f"), CourageThreshold);
        //UE_LOG(LogTemp, Warning, TEXT("RarityRoll: %f"), RarityRoll);


        //Default Essence to add multiper
        int EssenceToAdd = 1;
        if (RarityRoll <= WisdomThreshold)  // 50% chance for Wisdom
        {
            NewLog->EssenceRarity = "Wisdom";
            ExperienceGain = static_cast<float>(FMath::RoundToInt(10.0f * BonusManager->MultiplierSet.WisdomEssenceMultiplier));
            Character->ShowExpNumber(ExperienceGain, Character, FLinearColor::Blue);
            EssenceToAdd *= BonusManager->MultiplierSet.WisdomYieldMultiplier;
        }
        else if (RarityRoll <= TemperanceThreshold)  // 25% chance for Temperance
        {
            NewLog->EssenceRarity = "Temperance";
            ExperienceGain = static_cast<float>(FMath::RoundToInt(20.0f * BonusManager->MultiplierSet.TemperanceEssenceMultiplier));
            Character->ShowExpNumber(ExperienceGain, Character, FLinearColor::Green);
            EssenceToAdd *= BonusManager->MultiplierSet.TemperanceYieldMultiplier;
        }
        else if (RarityRoll <= JusticeThreshold)  // 20% chance for Justice
        {
            NewLog->EssenceRarity = "Justice";
            ExperienceGain = static_cast<float>(FMath::RoundToInt(30.0f * BonusManager->MultiplierSet.JusticeEssenceMultiplier));
            Character->ShowExpNumber(ExperienceGain, Character, FLinearColor::White);
            EssenceToAdd *= BonusManager->MultiplierSet.JusticeYieldMultiplier;
        }
        else // if (RarityRoll <= CourageThreshold)  // 5% chance for Courage
        {
            NewLog->EssenceRarity = "Courage";
            ExperienceGain = static_cast<float>(FMath::RoundToInt(50.0f * BonusManager->MultiplierSet.CourageEssenceMultiplier));
            Character->ShowExpNumber(ExperienceGain, Character, FLinearColor::Red);
            EssenceToAdd *= BonusManager->MultiplierSet.CourageYieldMultiplier;
        }

        AddExperience(ExperienceGain);


        // Load the current save game instance
        UIdleSaveGame * SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::LoadGameFromSlot("TotalEssenceGatheredSaveSlot", 0));
        if (SaveGameInstance)
        {
            UWorld* World = GetWorld();
            // Increment the essence count
            SaveGameInstance->IncrementEssenceCount(EssenceToAdd, World);
        }
        else
        {
            // If no save game exists, create a new one and then increment
            SaveGameInstance = Cast<UIdleSaveGame>(UGameplayStatics::CreateSaveGameObject(UIdleSaveGame::StaticClass()));
            UWorld* World = GetWorld();
            // Increment the essence count
            SaveGameInstance->IncrementEssenceCount(EssenceToAdd, World);
            // Save the new save game instance
            UGameplayStatics::SaveGameToSlot(SaveGameInstance, "TotalEssenceGatheredSaveSlot", 0);
        }

        // Update quest progress here
        //AIdleCharacter* Character = Cast<AIdleCharacter>(GetAvatarActorFromActorInfo());
        ADialogueManager* DialogueManager = ADialogueManager::GetInstance(GetWorld());
        if (Character)
        {
            if (Character->ActiveQuests.Num() > 0)
            {
                CurrentQuest = Character->ActiveQuests.Last();
            }
            


            //if (CurrentQuest && !CurrentQuest->bIsCompleted)
            //{
                // Update the progress based on the type of essence
                if (NewLog->EssenceRarity == "Wisdom")
                {
                    //CurrentQuest->UpdateProgress("Wisdom", EssenceToAdd);
                    Character->UpdateAllActiveQuests("Wisdom", EssenceToAdd * BonusManager->MultiplierSet.WisdomYieldMultiplier);
                    if(CurrentQuest)
                    DialogueManager->UpdateAndSaveQuestProgress(CurrentQuest, "Wisdom", EssenceToAdd);
                    //UE_LOG(LogTemp, Warning, TEXT("Wisdom added in quest"));
                }
                else if (NewLog->EssenceRarity == "Temperance")
                {
                    //CurrentQuest->UpdateProgress("Temperance", EssenceToAdd);
                    Character->UpdateAllActiveQuests("Temperance", EssenceToAdd * BonusManager->MultiplierSet.TemperanceYieldMultiplier);
                    if (CurrentQuest)
                    DialogueManager->UpdateAndSaveQuestProgress(CurrentQuest, "Temperance", EssenceToAdd);
                    //UE_LOG(LogTemp, Warning, TEXT("Temperance added in quest"));
                }
                else if (NewLog->EssenceRarity == "Justice")
                {
                    //CurrentQuest->UpdateProgress("Justice", EssenceToAdd);
                    Character->UpdateAllActiveQuests("Justice", EssenceToAdd * BonusManager->MultiplierSet.JusticeYieldMultiplier);
                    if (CurrentQuest)
                    DialogueManager->UpdateAndSaveQuestProgress(CurrentQuest, "Justice", EssenceToAdd);
                    //UE_LOG(LogTemp, Warning, TEXT("Justice added in quest"));
                }
                else if (NewLog->EssenceRarity == "Courage")
                {
                    //CurrentQuest->UpdateProgress("Courage", EssenceToAdd);
                    Character->UpdateAllActiveQuests("Courage", EssenceToAdd * BonusManager->MultiplierSet.CourageYieldMultiplier);
                    if (CurrentQuest)
                    DialogueManager->UpdateAndSaveQuestProgress(CurrentQuest, "Courage", EssenceToAdd);
                    //UE_LOG(LogTemp, Warning, TEXT("Courage added in quest"));
                }
                
                /*
                // Check if the quest is completed after updating
                if (CurrentQuest->IsQuestComplete())
                {
                    // Handle quest completion, e.g., give rewards, notify the player, etc.
                    Character->CompleteQuest(CurrentQuest);
                }
                */
            //}
            //else {
            //    UE_LOG(LogTemp, Warning, TEXT("CurrentQuest not valid"));
            //}
        }



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
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Essence data table not active yet"));
        }
        //AIdleCharacter* Character = Cast<AIdleCharacter>(GetAvatarActorFromActorInfo());
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

    //UE_LOG(LogTemp, Warning, TEXT("Global Before add: %f"), IdleAttributeSet->GetWoodcutExp());
    //UE_LOG(LogTemp, Warning, TEXT("Weekly Before add: %f"), IdleAttributeSet->GetWeeklyWoodcutExp());
    //UE_LOG(LogTemp, Warning, TEXT("Amount: %f"), Amount);
    IdleAttributeSet->SetWoodcutExp(IdleAttributeSet->GetWoodcutExp() + Amount);
    IdleAttributeSet->SetWeeklyWoodcutExp(IdleAttributeSet->GetWeeklyWoodcutExp() + Amount);
    //UE_LOG(LogTemp, Warning, TEXT("Global After add: %f"), IdleAttributeSet->GetWoodcutExp());
    //UE_LOG(LogTemp, Warning, TEXT("Weekly AFter add: %f"), IdleAttributeSet->GetWeeklyWoodcutExp());
}

void UWoodcuttingAbility::GetLegendaryEssence()
{
    AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
    AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
    ABonusManager* BonusManager = ABonusManager::GetInstance(GetWorld());

    UItem* NewLog = NewObject<UItem>();
    int EssenceToAdd = 1;

    NewLog->EssenceRarity = "Legendary";

    AIdleCharacter* Character = Cast<AIdleCharacter>(GetAvatarActorFromActorInfo());
    ADialogueManager* DialogueManager = ADialogueManager::GetInstance(GetWorld());
    if (CurrentQuest)
    {
        CurrentQuest = Character->ActiveQuests.Last();
        DialogueManager->UpdateAndSaveQuestProgress(CurrentQuest, "Legendary", EssenceToAdd);
    }
    Character->UpdateAllActiveQuests("Legendary", EssenceToAdd * BonusManager->MultiplierSet.CourageYieldMultiplier);

    ATestManager* TestManager = ATestManager::GetInstance(GetWorld());
    float LegendaryExpGain = 1000;
    
    ExperienceGain = static_cast<float>(FMath::RoundToInt(LegendaryExpGain * BonusManager->MultiplierSet.LegendaryEssenceMultiplier));
    Character->ShowExpNumber(ExperienceGain, Character, FLinearColor::Yellow);
    EssenceToAdd *= BonusManager->MultiplierSet.LegendaryYieldMultiplier;

    AddExperience(ExperienceGain);

    // Load the data table
    UDataTable* EssenceDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Blueprints/DataTables/DT_EssenceTypes.DT_EssenceTypes"));
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
    //AIdleCharacter* Character = Cast<AIdleCharacter>(GetAvatarActorFromActorInfo());
    for (int i = 0; i < EssenceToAdd; ++i)
    {
        Character->CharacterInventory->AddItem(NewLog);
    }

    UWorld* World = GetWorld();
    AIdleActorManager* IdleActorManager = AIdleActorManager::GetInstance(World);
    AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
    GameChatManager->PostNotificationToUI("You receive a legendary essence!", FLinearColor::Yellow);
    PC->CurrentTree->Tags.Remove("Legendary");

    ACofferManager* CofferManager = ACofferManager::GetInstance(GetWorld());
    CofferManager->RemoveActiveCoffers();
    IdleActorManager->DeactivateCurrentLegendaryTree();
}

void UWoodcuttingAbility::CallEndAbility()
{
    //EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
    //UE_LOG(LogTemp, Warning, TEXT("Call End ability from PlayerController"));
}

FActiveGameplayEffectHandle UWoodcuttingAbility::GetActiveEffectHandle() const
{
    return ActiveEffectHandle;
}

void UWoodcuttingAbility::StopCutDownTimer()
{
    FTimerManager& TimerManager = GetWorld()->GetTimerManager();
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
    //UE_LOG(LogTemp, Warning, TEXT("End ability in stop cutdowntimer"));

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

