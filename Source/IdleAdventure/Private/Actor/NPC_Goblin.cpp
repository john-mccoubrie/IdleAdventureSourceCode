


#include "Actor/NPC_Goblin.h"
#include <Combat/CombatManager.h>
#include "Combat/CharacterCombatComponent.h"
#include "Character/IdleCharacter.h"
#include <Kismet/GameplayStatics.h>

ANPC_Goblin::ANPC_Goblin()
{
	CombatComponent = CreateDefaultSubobject<UNPCCombatComponent>(TEXT("NPCCombatComponent"));

	//Set the default AI controller class
	NPCAIControllerClass = ANPCAIController::StaticClass();
}

void ANPC_Goblin::Interact()
{
	ACombatManager* CombatManager = ACombatManager::GetInstance(GetWorld());
	AIdleCharacter* PlayerCharacter = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	CombatManager->HandleCombat(PlayerCharacter->CombatComponent, this->CombatComponent, 0);
}
