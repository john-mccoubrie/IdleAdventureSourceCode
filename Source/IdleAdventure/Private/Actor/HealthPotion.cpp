


#include "Actor/HealthPotion.h"
#include <Character/IdleCharacter.h>
#include <Kismet/GameplayStatics.h>
#include <Player/IdlePlayerController.h>

AHealthPotion::AHealthPotion()
{
 	
}

void AHealthPotion::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Health potion interact called"));
	ConsumeHealthPotion();
}

void AHealthPotion::ConsumeHealthPotion()
{
	AIdleCharacter* Character = Cast<AIdleCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
	if (Character->CombatComponent->Health + 20 <= 500)
	{
		Character->CombatComponent->AddHealth(20.0f);
		Character->CombatComponent->OnHealthChanged.Broadcast(Character->CombatComponent->Health, Character->CombatComponent->MaxHealth);
		PC->IdleInteractionComponent->PlayPickupPotionSound();
	}
	else
	{
		Character->CombatComponent->AddHealth(500 - Character->CombatComponent->Health);
		Character->CombatComponent->OnHealthChanged.Broadcast(Character->CombatComponent->Health, Character->CombatComponent->MaxHealth);
		PC->IdleInteractionComponent->PlayPickupPotionSound();
	}
	Destroy();
}


