#include "Actor/IdleActorManager.h"
#include "EngineUtils.h"
#include "NiagaraComponent.h"
#include <Player/IdlePlayerController.h>
#include <Player/IdlePlayerState.h>

AIdleActorManager* AIdleActorManager::Instance = nullptr;

void AIdleActorManager::BeginPlay()
{
    Super::BeginPlay();

    GetLegendaryTree();
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

void AIdleActorManager::CutTree(AIdleEffectActor* Tree)
{
    //UE_LOG(LogTemp, Warning, TEXT("CutTree called for tree: %s"), *Tree->GetName());

    if (TreeChoppingStates.Contains(Tree->GetFName()) && TreeChoppingStates[Tree->GetFName()])
    {
        //UE_LOG(LogTemp, Warning, TEXT("Tree is already being chopped! in ActorManager"));
        return;
    }

    TreeChoppingStates.Add(Tree->GetFName(), true);
    
    FTreeRespawnInfo RespawnInfo;
    RespawnInfo.Location = Tree->GetActorLocation();
    RespawnInfo.Rotation = Tree->GetActorRotation();

    TreeRespawnMap.Add(Tree->GetFName(), RespawnInfo);

    if (TreeTimers.Contains(Tree->GetFName()))
    {
        FTimerHandle& ExistingTimerHandle = TreeTimers[Tree->GetFName()];
        if (ExistingTimerHandle.IsValid())
        {
            GetWorld()->GetTimerManager().ClearTimer(ExistingTimerHandle);
            ExistingTimerHandle.Invalidate();
        }
    }

    FTimerHandle LocalTreeTimerHandle;
    float TimeUntilRespawn = Tree->TotalDuration + 10;

    GetWorld()->GetTimerManager().SetTimer(LocalTreeTimerHandle, FTimerDelegate::CreateUObject(this, &AIdleActorManager::RespawnTree, Tree->GetFName()), TimeUntilRespawn, false);
    float TimeRemainingBeforeReset = GetWorld()->GetTimerManager().GetTimerRemaining(LocalTreeTimerHandle);
    //UE_LOG(LogTemp, Warning, TEXT("ActorManagerTimer: %f"), TimeRemainingBeforeReset);
    //UE_LOG(LogTemp, Warning, TEXT("set timer called for %s"), Tree->GetFName().ToString());

    TreeTimers.Add(Tree->GetFName(), LocalTreeTimerHandle);
}

void AIdleActorManager::RespawnTree(FName TreeName)
{
    if (TreeRespawnMap.Contains(TreeName))
    {
        FTreeRespawnInfo RespawnInfo = TreeRespawnMap[TreeName];


        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
        // Spawn the tree without specifying a name
        AIdleEffectActor* NewTree = GetWorld()->SpawnActor<AIdleEffectActor>(PS->TreeBlueprintClass, RespawnInfo.Location, RespawnInfo.Rotation);
        //GetWorld()->SpawnActor<AIdleEffectActor>(TreeName->GetClass(), RespawnInfo.Location, RespawnInfo.Rotation);
        //UE_LOG(LogTemp, Warning, TEXT("Tree respawn called"));

        if (NewTree)
        {
            // Increment the counter for each new tree spawned
            TreeCounterName++;

            // Create a unique name for the new tree using the counter
            FString UniqueName = FString::Printf(TEXT("NewTree_%d"), TreeCounterName);
            NewTree->Rename(*UniqueName);

            TreeRespawnMap.Remove(TreeName);
            //UE_LOG(LogTemp, Warning, TEXT("Tree removed: %s"), *TreeName.ToString());
            TreeChoppingStates.Add(*UniqueName, false);
            //UE_LOG(LogTemp, Warning, TEXT("Tree added: %s"), *UniqueName);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Does not contain Tree: %s"), *TreeName.ToString());
    }
}

void AIdleActorManager::GetLegendaryTree()
{
    // Ensure the array is empty before populating it
    AllIdleEffectActors.Empty();

    for (TActorIterator<AIdleEffectActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
        AllIdleEffectActors.Add(*ActorItr);
    }

    // Ensure there is at least one IdleEffectActor in the level
    if (AllIdleEffectActors.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, AllIdleEffectActors.Num() - 1);
        LegendaryIdleEffectActor = AllIdleEffectActors[RandomIndex];

        // Log the name of the LegendaryIdleEffectActor to the output log
        UE_LOG(LogTemp, Warning, TEXT("LegendaryIdleEffectActor is: %s"), *LegendaryIdleEffectActor->GetName());

        // Check if the actor is valid before trying to call a method on it
        if (LegendaryIdleEffectActor)
        {
            // Activate the legendary effect particle system
            LegendaryIdleEffectActor->ActivateLegendaryEffect();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("LegendaryIdleEffectActor is INVALID!"));
        }
    }
    else
    {
        // Log a warning if no IdleEffectActor instances are found
        UE_LOG(LogTemp, Warning, TEXT("No IdleEffectActor instances found in the level!"));
    }
}

void AIdleActorManager::SelectNewLegendaryTree()
{
    // Ensure there is at least one IdleEffectActor in the level
    if (AllIdleEffectActors.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, AllIdleEffectActors.Num() - 1);
        LegendaryIdleEffectActor = AllIdleEffectActors[RandomIndex];

        // Log the name of the LegendaryIdleEffectActor to the output log
        UE_LOG(LogTemp, Warning, TEXT("New LegendaryIdleEffectActor is: %s"), *LegendaryIdleEffectActor->GetName());

        // Check if the actor is valid before trying to call a method on it
        if (LegendaryIdleEffectActor)
        {
            // Activate the legendary effect particle system
            LegendaryIdleEffectActor->ActivateLegendaryEffect();
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