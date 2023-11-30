


#include "Animation/IdleAnimInstance.h"
#include <Player/IdlePlayerController.h>
/*
void UIdleAnimInstance::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent)
{
    if (NotifyEvent.NotifyName == TEXT("AttackNotify"))
    {
        // Get the owning actor of the SkeletalMeshComponent
        AActor* Owner = MeshComp->GetOwner();
        if (Owner)
        {
            // Cast the owner to APawn if it is one
            APawn* OwnerPawn = Cast<APawn>(Owner);
            if (OwnerPawn)
            {
                // Get the controller and cast it to your specific controller class
                AController* Controller = OwnerPawn->GetController();
                AIdlePlayerController* IdleController = Cast<AIdlePlayerController>(Controller);
                if (IdleController)
                {
                    // Call the function with the pawn
                    IdleController->StartAnimNotifyEnemyInteraction(OwnerPawn);
                }
            }
        }
    }
}
*/
