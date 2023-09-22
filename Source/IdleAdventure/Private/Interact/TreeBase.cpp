#include "Interact/TreeBase.h"
#include "IdleAdventure/IdleAdventure.h"


ATreeBase::ATreeBase()
{
	TreeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TreeMesh"));
	//TreeMesh->SetupAttachment(GetMesh());
	TreeMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	//GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

}

void ATreeBase::HighlightActor()
{
	TreeMesh->SetRenderCustomDepth(true);
	TreeMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	//GetMesh()->SetRenderCustomDepth(true);
	//GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void ATreeBase::UnHighlightActor()
{
	TreeMesh->SetRenderCustomDepth(false);
	//GetMesh()->SetRenderCustomDepth(false);
}
