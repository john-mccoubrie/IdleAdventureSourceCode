


#include "AbilitySystem/Abilities/ConversionAbility.h"
#include <Player/IdlePlayerState.h>
#include "GameplayEffectTypes.h"
#include "Player/IdlePlayerController.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEffectRemoved.h"
#include "Character/IdleCharacter.h"
#include <AbilitySystemBlueprintLibrary.h>
#include <Actor/Coffer.h>
#include <Kismet/GameplayStatics.h>
#include <AbilitySystem/Abilities/ConversionAbilityRemoved.h>
#include <Actor/CofferManager.h>

UConversionAbility::UConversionAbility()
{
	//UE_LOG(LogTemp, Warning, TEXT("UConversionAbility constructed: %p"), this);
}

UConversionAbility::~UConversionAbility()
{
	//UE_LOG(LogTemp, Warning, TEXT("UConversionAbility destroyed: %p"), this);
}

void UConversionAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Warning, TEXT("Activated Conversion Ability"));

	AIdleCharacter* Character = Cast<AIdleCharacter>(ActorInfo->AvatarActor.Get());
	UAnimMontage* AnimMontage = Character->CofferMontage;
	//Character->PlayAnimMontage(AnimMontage);

	CovertEssenceToEXP();


}

void UConversionAbility::CovertEssenceToEXP()
{
	AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
	AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
	ACofferManager* CofferManager = ACofferManager::GetInstance(GetWorld());

	ACoffer* ClickedCoffer = PC->ClickedCoffer;

	AIdleCharacter* Character = Cast<AIdleCharacter>(GetAvatarActorFromActorInfo());

	
			
	PS->AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PS);
	FGameplayEffectContextHandle EffectContextHandle = PS->AbilitySystemComponent->MakeEffectContext();
	const FGameplayEffectSpecHandle EffectSpecHandle = PS->AbilitySystemComponent->MakeOutgoingSpec(PC->ConversionGameplayEffect, 1.f, EffectContextHandle);
	FActiveGameplayEffectHandle EffectHandle = PS->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	//Set new duraiton based on the amount of essence in the player's inventory
	float NewDuration = Character->CharacterInventory->EssenceCountForDurationCalc * 10.0f;
	//UE_LOG(LogTemp, Warning, TEXT("EssenceCount: %f"), Character->CharacterInventory->EssenceCountForDurationCalc);
	//UE_LOG(LogTemp, Warning, TEXT("New Duration: %f"), NewDuration);
	// Remove the old effect (cannot edit existing const effect)
	PS->AbilitySystemComponent->RemoveActiveGameplayEffect(EffectHandle);

	// Create a new effect spec with the modified duration
	FGameplayEffectSpecHandle NewEffectSpecHandle = PS->AbilitySystemComponent->MakeOutgoingSpec(PC->ConversionGameplayEffect, 1.f, EffectContextHandle);
	NewEffectSpecHandle.Data->SetDuration(NewDuration, true);

	// Apply the new effect
	FActiveGameplayEffectHandle NewEffectHandle = PS->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewEffectSpecHandle.Data.Get());
	
	float CofferDuration = PS->AbilitySystemComponent->GetActiveGameplayEffect(NewEffectHandle)->GetDuration();
	//UE_LOG(LogTemp, Warning, TEXT("Coffer Duration: %f"), CofferDuration);
	//ClickedCoffer->StartExperienceTimer(CofferDuration);
	CofferManager->StartExperienceTimer(CofferDuration);
		
	// Built in delegate that calls "OnDurationEffectRemoved" When the conversion gameplay effect's duration automatically removes it
	FOnActiveGameplayEffectRemoved_Info* WaitEffectRemovedDelegate = PS->AbilitySystemComponent->OnGameplayEffectRemoved_InfoDelegate(NewEffectHandle);
	WaitEffectRemovedDelegate->AddUObject(this, &UConversionAbility::OnDurationEffectRemoved, CofferDuration, ClickedCoffer);
				
	//reset the essence count for tracking inventory
	UItem* Essence = NewObject<UItem>();
	Character->CharacterInventory->RemoveItem(Essence);
	Character->EssenceCount = 0;
	//UE_LOG(LogTemp, Warning, TEXT("EssenceCount: %f"), Character->EssenceCount);	
}

void UConversionAbility::OnDurationEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo, float CofferDuration, ACoffer* ClickedCoffer)
{
	//UE_LOG(LogTemp, Warning, TEXT("OnDurationEffectRemoved"));
	APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
	ACofferManager* CofferManager = ACofferManager::GetInstance(GetWorld());

	CofferManager->RemoveActiveCoffer(ClickedCoffer);

	// Get all ACoffer instances
	TArray<AActor*> TempCoffers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACoffer::StaticClass(), TempCoffers);
	//CofferManager->ActiveCoffers.Remove(ClickedCoffer);
	UE_LOG(LogTemp, Warning, TEXT("Removed Coffer: %s"), *ClickedCoffer->GetName());
	//UE_LOG(LogTemp, Warning, TEXT("Active Coffers: %d"), PS->ActiveCoffers.Num());
}
