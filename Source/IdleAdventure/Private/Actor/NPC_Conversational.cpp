


#include "Actor/NPC_Conversational.h"
#include <Quest/DialogueManager.h>

void ANPC_Conversational::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("NPC_Conversational Interact"));

	// Get current data table entry (pull key from save data)
	ADialogueManager* DialogueManager = ADialogueManager::GetInstance(GetWorld());
	// Maybe create widget (broadcast to UI)
	OnGetDialogueStep.Broadcast(DialogueManager->GetCurrentDialogue());
}
