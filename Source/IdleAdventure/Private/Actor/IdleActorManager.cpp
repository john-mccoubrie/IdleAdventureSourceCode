#include "Actor/IdleActorManager.h"
#include "EngineUtils.h"
#include "NiagaraComponent.h"
#include "Styling/SlateColor.h"
#include <Player/IdlePlayerController.h>
#include <Player/IdlePlayerState.h>
#include <Chat/GameChatManager.h>
#include <Test/TestManager.h>

AIdleActorManager* AIdleActorManager::Instance = nullptr;

void AIdleActorManager::BeginPlay()
{
    Super::BeginPlay();

    //GetLegendaryTree();
}

void AIdleActorManager::BeginDestroy()
{
    Super::BeginDestroy();
    //UE_LOG(LogTemp, Warning, TEXT("AIdleActorManager is being destroyed"));
    ResetInstance();
}

void AIdleActorManager::ResetInstance()
{
    Instance = nullptr;
}

void AIdleActorManager::StartTreeCountdown(AIdleEffectActor* Tree, float TreeLifeSpan)
{
    UE_LOG(LogTemp, Warning, TEXT("StartTreeCountdownTimer: %f"), TreeLifeSpan);

    // Check if a timer already exists for the tree
    if (TreeTimers.Contains(Tree->GetFName()))
    {
        FTimerHandle& ExistingTimerHandle = TreeTimers[Tree->GetFName()];
        if (ExistingTimerHandle.IsValid())
        {
            GetWorld()->GetTimerManager().ClearTimer(ExistingTimerHandle);
            ExistingTimerHandle.Invalidate();
        }
    }

    // Set up a new timer to call OnCountdownFinished after the specified duration
    FTimerHandle NewTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(NewTimerHandle, FTimerDelegate::CreateUObject(this, &AIdleActorManager::OnCountdownFinished, Tree), TreeLifeSpan, false);
    TreeTimers.Add(Tree->GetFName(), NewTimerHandle);
}

void AIdleActorManager::OnCountdownFinished(AIdleEffectActor* Tree)
{
    //UE_LOG(LogTemp, Warning, TEXT("OnCountdownFinished"));
    if (Tree)
    {
        // Hide the tree
        Tree->SetActorHiddenInGame(true);
        // Disable the tree's collision
        Tree->SetActorEnableCollision(false);
        //Disable input for the tree
        Tree->DisableInput(nullptr);


        //UE_LOG(LogTemp, Warning, TEXT("Hide Tree: %s"), *Tree->GetName());

        ATestManager* TestManager = ATestManager::GetInstance(GetWorld());
        float RespawnDelay = TestManager->CurrentSettings.TreeRespawnDelay;
        FTimerHandle NewTimerHandle;

        //Enable to enable tree respawning
        //GetWorld()->GetTimerManager().SetTimer(NewTimerHandle, FTimerDelegate::CreateUObject(this, &AIdleActorManager::RespawnTree, Tree), RespawnDelay, false);
        //TreeTimers.Add(Tree->GetFName(), NewTimerHandle);
    }
}

void AIdleActorManager::ResetTreeTimer(AIdleEffectActor* Tree)
{
    if (!Tree) return; // Ensure the tree is valid

    FName TreeName = Tree->GetFName();

    if (TreeTimers.Contains(TreeName))
    {
        FTimerHandle& ExistingTimerHandle = TreeTimers[TreeName];
        if (ExistingTimerHandle.IsValid())
        {
            GetWorld()->GetTimerManager().ClearTimer(ExistingTimerHandle);
            ExistingTimerHandle.Invalidate();
            UE_LOG(LogTemp, Warning, TEXT("Timer invalidated for tree: %s"), *TreeName.ToString());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No timer found for tree: %s"), *TreeName.ToString());
    }
}

void AIdleActorManager::CutTree(AIdleEffectActor* Tree)
{
    //UE_LOG(LogTemp, Warning, TEXT("CutTree called for tree and added to respawninfo: %s"), *Tree->GetName());

    //if (TreeChoppingStates.Contains(Tree->GetFName()) && TreeChoppingStates[Tree->GetFName()])
    //{
        //UE_LOG(LogTemp, Warning, TEXT("Tree is already being chopped! in ActorManager"));
        //return;
    //}

    TreeChoppingStates.Add(Tree->GetFName(), true);
    
    FTreeRespawnInfo RespawnInfo;
    RespawnInfo.Location = Tree->GetActorLocation();
    RespawnInfo.Rotation = Tree->GetActorRotation();
    RespawnInfo.TreeActor = Tree;

    TreeRespawnMap.Add(Tree->GetFName(), RespawnInfo);

    if (TreeTimers.Contains(Tree->GetFName()))
    {
        // If a timer exists, clear and invalidate it
        FTimerHandle& ExistingTimerHandle = TreeTimers[Tree->GetFName()];
        if (ExistingTimerHandle.IsValid())
        {
            GetWorld()->GetTimerManager().ClearTimer(ExistingTimerHandle);
            ExistingTimerHandle.Invalidate();
        }
    }

    // Use the same handle if it already exists, or create a new one if not
    FTimerHandle& TreeTimerHandle = TreeTimers.FindOrAdd(Tree->GetFName());
    float TimeUntilRespawn = Tree->TotalDuration;
    GetWorld()->GetTimerManager().SetTimer(TreeTimerHandle, FTimerDelegate::CreateUObject(this, &AIdleActorManager::OnCountdownFinished, Tree), TimeUntilRespawn, false);
    UE_LOG(LogTemp, Warning, TEXT("Total Duration: %f"), Tree->TotalDuration);
}

void AIdleActorManager::RespawnTree(AIdleEffectActor* Tree)
{
    if (Tree && TreeRespawnMap.Contains(Tree->GetFName()))
    {
        FTreeRespawnInfo RespawnInfo = TreeRespawnMap[Tree->GetFName()];

        // Unhide the tree
        RespawnInfo.TreeActor->SetActorHiddenInGame(false);
        //UE_LOG(LogTemp, Warning, TEXT("Respawned Tree: %s"), *Tree->GetName());

        // Enable the tree's collision
        RespawnInfo.TreeActor->SetActorEnableCollision(true);

        // Enable input for the tree
        RespawnInfo.TreeActor->EnableInput(nullptr); // Pass nullptr if the tree doesn't have a specific player controller

        // Update tree state
        TreeChoppingStates.Add(Tree->GetFName(), false);

        // Remove the tree's respawn info since it has respawned
        TreeRespawnMap.Remove(Tree->GetFName());

        // Clear the timer associated with this tree
        if (TreeTimers.Contains(Tree->GetFName()))
        {
            FTimerHandle& TimerHandle = TreeTimers[Tree->GetFName()];
            if (TimerHandle.IsValid())
            {
                GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
                TimerHandle.Invalidate();
            }
            TreeTimers.Remove(Tree->GetFName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to respawn tree: %s"), Tree ? *Tree->GetName() : TEXT("nullptr"));
    }
}

void AIdleActorManager::GetLegendaryTree()
{
    // Ensure the array is empty before populating it
    AllIdleEffectActors.Empty();

    // Iterate over all AIdleEffectActor instances in the world
    for (TActorIterator<AIdleEffectActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
        // Check if the actor is not hidden in game
        if (!ActorItr->IsHidden())
        {
            // Add the actor to the array if it's visible
            AllIdleEffectActors.Add(*ActorItr);
        }
    }

    // Ensure there is at least one IdleEffectActor in the level
    if (AllIdleEffectActors.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, AllIdleEffectActors.Num() - 1);
        LegendaryIdleEffectActor = AllIdleEffectActors[RandomIndex];
        LegendaryIdleEffectActor->Tags.Add("Legendary");

        // Check if the actor is valid before trying to call a method on it
        if (LegendaryIdleEffectActor)
        {
            // Create message in game chat
            AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
            if (!GameChatManager) {
                UE_LOG(LogTemp, Error, TEXT("GameChatManager is null in idleactor manager for legendary tree!"));
            }
            GameChatManager->PostNotificationToUI(TEXT("A Legendary Tree spawned in the world..."), FLinearColor::Yellow);

            // Log the name of the LegendaryIdleEffectActor to the output log
            UE_LOG(LogTemp, Warning, TEXT("LegendaryIdleEffectActor is: %s"), *LegendaryIdleEffectActor->GetName());

            // Activate the legendary effect particle system
            LegendaryIdleEffectActor->ActivateLegendaryEffect();
            AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
            PC->IdleInteractionComponent->PlayLegendaryTreeSpawnSound();
            UE_LOG(LogTemp, Warning, TEXT("Legendary Tree Spawned"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("LegendaryIdleEffectActor is INVALID!"));
        }
    }
    else
    {
        // Log a warning if no IdleEffectActor instances are found
        UE_LOG(LogTemp, Warning, TEXT("No visible idleeffectactors found in this level"));
    }
}

void AIdleActorManager::SelectNewLegendaryTree()
{
    DeactivateCurrentLegendaryTree();
    // Ensure there is at least one IdleEffectActor in the level
    if (AllIdleEffectActors.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, AllIdleEffectActors.Num() - 1);
        LegendaryIdleEffectActor = AllIdleEffectActors[RandomIndex];
        LegendaryIdleEffectActor->Tags.Add("Legendary");

        // Log the name of the LegendaryIdleEffectActor to the output log
        //UE_LOG(LogTemp, Warning, TEXT("New LegendaryIdleEffectActor is: %s"), *LegendaryIdleEffectActor->GetName());

        // Check if the actor is valid before trying to call a method on it
        if (LegendaryIdleEffectActor)
        {
            // Activate the legendary effect particle system
            LegendaryIdleEffectActor->ActivateLegendaryEffect();
            AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
            GameChatManager->PostNotificationToUI(TEXT("A Legendary Tree spawned in the world..."), FLinearColor::Yellow);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("New LegendaryIdleEffectActor is INVALID!"));
        }
    }
    else
    {
        // Log a warning if no IdleEffectActor instances are found
        UE_LOG(LogTemp, Warning, TEXT("No IdleEffectActor instances found in the level!"));
    }
}

void AIdleActorManager::DeactivateCurrentLegendaryTree()
{
    if (LegendaryIdleEffectActor)
    {
        LegendaryIdleEffectActor->DeactivateLegendaryEffect();
        LegendaryIdleEffectActor->Tags.Remove("Legendary");
    }
}

AIdleActorManager* AIdleActorManager::GetInstance(UWorld* World)
{
    if (!Instance)
    {
        for (TActorIterator<AIdleActorManager> It(World); It; ++It)
        {
            Instance = *It;
            break;
        }
        if (!Instance)
        {
            Instance = World->SpawnActor<AIdleActorManager>();
        }
    }
    return Instance;
}