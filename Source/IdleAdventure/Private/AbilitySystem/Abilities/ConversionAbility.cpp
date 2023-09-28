


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
	//UE_LOG(LogTemp, Warning, TEXT("Activated Conversion Ability"));
	CovertEssenceToEXP();	
}

void UConversionAbility::CovertEssenceToEXP()
{
	AIdlePlayerController* PC = Cast<AIdlePlayerController>(GetWorld()->GetFirstPlayerController());
	AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();

	ACoffer* ClickedCoffer = PC->ClickedCoffer;

	if (PS->ActiveCoffers.Contains(ClickedCoffer))
	{
		//this coffer is already active
		UE_LOG(LogTemp, Warning, TEXT("The Coffer is already active"));
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("The Coffer is already active"));
		}


		return;
	}


	AIdleCharacter* Character = Cast<AIdleCharacter>(GetAvatarActorFromActorInfo());

	if (Character->EssenceCount >= 3)
	{
		
			
				PS->AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PS);
				FGameplayEffectContextHandle EffectContextHandle = PS->AbilitySystemComponent->MakeEffectContext();
				const FGameplayEffectSpecHandle EffectSpecHandle = PS->AbilitySystemComponent->MakeOutgoingSpec(PC->ConversionGameplayEffect, 1.f, EffectContextHandle);
				FActiveGameplayEffectHandle EffectHandle = PS->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
				float CofferDuration = PS->AbilitySystemComponent->GetActiveGameplayEffect(EffectHandle)->GetDuration();
				// Get all ACoffer instances
				TArray<AActor*> Coffers;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACoffer::StaticClass(), Coffers);
				

				
				
				if (PS->ActiveCoffers.Num() < 3)
				{
					ClickedCoffer->StartExperienceTimer(CofferDuration);
					PS->ActiveCoffers.Add(ClickedCoffer);
					//UE_LOG(LogTemp, Warning, TEXT("Active Coffers: %d"), PS->ActiveCoffers.Num());
					//UE_LOG(LogTemp, Warning, TEXT("Address of ClickedCoffer: %p"), ClickedCoffer);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Maximum number of coffers is reached"));
				}
				
				

				// Built in delegate that calls "OnDurationEffectRemoved" When the conversion gameplay effect's duration automatically removes it
				FOnActiveGameplayEffectRemoved_Info* WaitEffectRemovedDelegate = PS->AbilitySystemComponent->OnGameplayEffectRemoved_InfoDelegate(EffectHandle);
				WaitEffectRemovedDelegate->AddUObject(this, &UConversionAbility::OnDurationEffectRemoved, CofferDuration, ClickedCoffer);
				
				UItem* Essence = NewObject<UItem>();
				Character->CharacterInventory->RemoveItem(Essence);
				Character->EssenceCount = 0;
				//UE_LOG(LogTemp, Warning, TEXT("EssenceCount: %f"), Character->EssenceCount);
			
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough essence in conversion ability!"));
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Too little or too few essence, must have at least 3."));
		}
	}
}

void UConversionAbility::OnDurationEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo, float CofferDuration, ACoffer* ClickedCoffer)
{
	//UE_LOG(LogTemp, Warning, TEXT("OnDurationEffectRemoved"));
	APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();

	// Get all ACoffer instances
	TArray<AActor*> Coffers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACoffer::StaticClass(), Coffers);
	PS->ActiveCoffers.Remove(ClickedCoffer);
	//UE_LOG(LogTemp, Warning, TEXT("Active Coffers: %d"), PS->ActiveCoffers.Num());
}
