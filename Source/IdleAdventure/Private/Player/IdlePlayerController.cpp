#include "Player/IdlePlayerController.h"
#include "..\..\Public\Player\IdlePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Input/IdleInputComponent.h"
#include "Interact/TargetInterface.h"
#include "TimerManager.h"
#include "AIController.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NavigationSystem.h"
#include "EngineUtils.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "DrawDebugHelpers.h"
#include "Components/SplineComponent.h"
#include "Actor/IdleActorManager.h"
#include "Character/IdleCharacter.h"
#include "AbilitySystem/Abilities/WoodcuttingAbility.h"
#include <Player/IdlePlayerState.h>
#include <Actor/IdleEffectActor.h>
#include <Kismet/GameplayStatics.h>
#include <AbilitySystemBlueprintLibrary.h>
#include <Actor/Coffer.h>
#include "AIController.h"
#include <Kismet/KismetMathLibrary.h>

AIdlePlayerController::AIdlePlayerController()
{
	bReplicates = true;

	WoodcuttingCastingDistance = 400.0f;
	CofferCastingDistance = 500.0f;
	ZMultiplierStaffEndLoc = 80.f;
	XMultiplierStaffEndLoc = 20.f;
	YMultiplierStaffEndLoc = -20.f;
	ZMultiplierTreeLoc = 0.f;
	//YawRotationStaffMultiplier = 20.f;

	bHasPerformedCofferClick = false;
}

void AIdlePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// 1. Check if PlayerTick is being called
	//UE_LOG(LogTemp, Warning, TEXT("PlayerTick called"));

	// 2. Check the value of TargetTree
	//UE_LOG(LogTemp, Warning, TEXT("TargetTree is: %s"), TargetTree ? *TargetTree->GetName() : TEXT("Null"));

	// 3. Check the boolean flags
	//UE_LOG(LogTemp, Warning, TEXT("bIsMovingToTarget: %s, bIsChoppingTree: %s"),
		//bIsMovingToTarget ? TEXT("True") : TEXT("False"),
		//bIsChoppingTree ? TEXT("True") : TEXT("False"));

	if (bIsMovingToTarget && !bIsChoppingTree && TargetTree)
	{
		APawn* ControlledPawn = GetPawn();

		// 4. Pawn null check
		if (!ControlledPawn)
		{
			UE_LOG(LogTemp, Warning, TEXT("ControlledPawn is null"));
			return;
		}

		FVector CurrentLocation = ControlledPawn->GetActorLocation();

		// 5. Distance check
		//UE_LOG(LogTemp, Warning, TEXT("Pawn location: %s, Tree location: %s"),
			//*CurrentLocation.ToString(),
			//*TargetTree->GetActorLocation().ToString());

		FVector PawnLocation2D = FVector(CurrentLocation.X, CurrentLocation.Y, 0);
		FVector TreeLocation2D = FVector(TargetTree->GetActorLocation().X, TargetTree->GetActorLocation().Y, 0);
		if (FVector::Distance(PawnLocation2D, TreeLocation2D) <= WoodcuttingCastingDistance)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Arrived close to tree"));
			StartWoodcuttingAbility(ControlledPawn);
			//UE_LOG(LogTemp, Warning, TEXT("StartWoodcutAbility"));
			return;
		}
	}

	//move to coffer
	if (bIsMovingToCoffer && !bHasPerformedCofferClick && TargetCoffer)
	{
		APawn* ControlledPawn = GetPawn();

		if (!ControlledPawn)
		{
			UE_LOG(LogTemp, Warning, TEXT("ControlledPawn is null"));
			return;
		}

		FVector CurrentLocation = ControlledPawn->GetActorLocation();
		FVector PawnLocation2D = FVector(CurrentLocation.X, CurrentLocation.Y, 0);
		FVector CofferLocation2D = FVector(TargetCoffer->GetActorLocation().X, TargetCoffer->GetActorLocation().Y, 0);

		if (FVector::Distance(PawnLocation2D, CofferLocation2D) <= CofferCastingDistance)
		{
			
			StartConversionAbility(ControlledPawn);
			return;
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("one of the bools or targetcoffer is false"));
	}
}

void AIdlePlayerController::BeginPlay()
{
	Super::BeginPlay();
	//UE_LOG(LogTemp, Warning, TEXT("AIdlePlayerController::BeginPlay() called"));

	AIdleActorManager::GetInstance(GetWorld());
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
	AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PS);

	
	FGameplayAbilityInfo WoodcuttingAbilityInfo;
	WoodcuttingAbilityInfo.Ability = UWoodcuttingAbility::StaticClass();
	WoodcuttingAbilityInfo.Level = 1;
	WoodcuttingAbilityInfo.InputID = 0;

	PS->GiveAbility(WoodcuttingAbilityInfo);
	
	if (!CurrentWoodcuttingAbilityInstance)
	{
		CurrentWoodcuttingAbilityInstance = NewObject<UWoodcuttingAbility>(this, UWoodcuttingAbility::StaticClass());
		//UE_LOG(LogTemp, Warning, TEXT("CurrentWoodcuttingInstance created in playercontroller begin play"));
	}
	
	FGameplayAbilityInfo ConversionAbilityInfo;
	ConversionAbilityInfo.Ability = UConversionAbility::StaticClass();
	ConversionAbilityInfo.Level = 1;
	ConversionAbilityInfo.InputID = 0;
	
	//give the ability to the player state to create its instance
	PS->GiveAbility(ConversionAbilityInfo);

	check(IdleContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(IdleContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AIdlePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	//EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AIdlePlayerController::Move);
	//EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &AIdlePlayerController::ClickTree);
	//EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &AIdlePlayerController::OnCofferClicked);
	EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &AIdlePlayerController::HandleClickAction);
	EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Started, this, &AIdlePlayerController::HandleZoomAction);
	EnhancedInputComponent->BindAction(RotateHorizontalAction, ETriggerEvent::Triggered, this, &AIdlePlayerController::RotateHorizontal);
	EnhancedInputComponent->BindAction(RotateVerticalAction, ETriggerEvent::Triggered, this, &AIdlePlayerController::RotateVertical);
}

void AIdlePlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AIdlePlayerController::RotateHorizontal(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		USpringArmComponent* SpringArm = Cast<USpringArmComponent>(ControlledPawn->GetComponentByClass(USpringArmComponent::StaticClass()));
		if (SpringArm)
		{
			float RotationSpeed = 100.0f; // Adjust this value for rotation speed
			float InputValue = InputActionValue.Get<float>();
			SpringArm->AddWorldRotation(FRotator(0, InputValue * RotationSpeed * GetWorld()->GetDeltaSeconds(), 0));
		}
	}
}

void AIdlePlayerController::RotateVertical(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		USpringArmComponent* SpringArm = Cast<USpringArmComponent>(ControlledPawn->GetComponentByClass(USpringArmComponent::StaticClass()));
		if (SpringArm)
		{
			float RotationSpeed = 100.0f; // Adjust this value for rotation speed
			float InputValue = InputActionValue.Get<float>();

			// Calculate the proposed new vertical rotation
			float ProposedVerticalRotation = VerticalRotation + InputValue * RotationSpeed * GetWorld()->GetDeltaSeconds();

			// Clamp the proposed vertical rotation within bounds
			ProposedVerticalRotation = FMath::Clamp(ProposedVerticalRotation, MinVerticalRotation, MaxVerticalRotation);

			// Calculate the difference that we should actually rotate
			float ActualRotationDifference = ProposedVerticalRotation - VerticalRotation;

			// Apply the rotation difference to the spring arm
			SpringArm->AddLocalRotation(FRotator(ActualRotationDifference, 0, 0));

			// Update our current vertical rotation
			VerticalRotation = ProposedVerticalRotation;
		}
	}
}



void AIdlePlayerController::HandleClickAction(const FInputActionValue& InputActionValue)
{
	//UE_LOG(LogTemp, Warning, TEXT("HandleClickAction"));
	FHitResult ClickResult;
	GetHitResultUnderCursor(ECC_Visibility, false, ClickResult);
	APawn* PlayerPawn = GetPawn<APawn>();

	//UE_LOG(LogTemp, Warning, TEXT("Object clicked on: %s"), *ClickResult.GetActor()->GetName());

	if (ClickResult.GetComponent()->ComponentTags.Contains("Tree"))
	{
		bHasPerformedCofferClick = false;
		TreeClickEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TreeClickEffectSystem, ClickResult.Location);
		CurrentTree = Cast<AIdleEffectActor>(ClickResult.GetActor());
		ClickTree(ClickResult, PlayerPawn);
	}
	else if (ClickResult.GetComponent()->ComponentTags.Contains("Coffer"))
	{
		CofferClickEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CofferClickEffectSystem, ClickResult.Location);
		if (bIsChoppingTree)
		{
			bIsChoppingTree = false;
			bHasPerformedCofferClick = false;
			InterruptTreeCutting();
			//ResetTreeTimer(CurrentTree);
			AIdleActorManager* TreeManager = nullptr;
			for (TActorIterator<AIdleActorManager> It(GetWorld()); It; ++It)
			{
				TreeManager = *It;
				break; // Exit the loop once the Tree Manager is found
			}
			if (TreeManager)
			{
				TreeManager->ResetTreeTimer(CurrentTree);
			}
			ResetWoodcuttingAbilityTimer();
			//OnCofferClicked(ClickResult, PlayerPawn);

			//if (CurrentWoodcuttingAbilityInstance && CurrentWoodcuttingAbilityInstance->bAbilityIsActive)
			//{
				CurrentWoodcuttingAbilityInstance->bAbilityIsActive = false;
				APlayerController* PC = GetWorld()->GetFirstPlayerController();
				AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
				PS->AbilitySystemComponent->OnPeriodicGameplayEffectExecuteDelegateOnSelf.RemoveAll(CurrentWoodcuttingAbilityInstance);
				UE_LOG(LogTemp, Warning, TEXT("CurrentWoodcuttingAbilityInstance removed periodicgameplayeffectdelegate in playercontroller"));
			//}
			//else
			//{
				//UE_LOG(LogTemp, Warning, TEXT("CurrentWoodcuttingAbilityInstance is null ptr in PlayerController"));
			//}
			CurrentTree->SetLifeSpan(-1.0f);	
		}
		
		OnCofferClicked(ClickResult, PlayerPawn);
		
	}
	else
	{
		MouseClickEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MouseClickEffectSystem, ClickResult.Location);
		MoveToClickLocation(InputActionValue, ClickResult, PlayerPawn);
		if (bIsChoppingTree)
		{
			bIsChoppingTree = false;
			bHasPerformedCofferClick = false;
			InterruptTreeCutting();
			//ResetTreeTimer(CurrentTree);
			AIdleActorManager* TreeManager = nullptr;
			for (TActorIterator<AIdleActorManager> It(GetWorld()); It; ++It)
			{
				TreeManager = *It;
				break; // Exit the loop once the Tree Manager is found
			}
			if (TreeManager)
			{
				TreeManager->ResetTreeTimer(CurrentTree);
			}
			ResetWoodcuttingAbilityTimer();

			if (CurrentWoodcuttingAbilityInstance && CurrentWoodcuttingAbilityInstance->bAbilityIsActive)
			{
				CurrentWoodcuttingAbilityInstance->bAbilityIsActive = false;
				APlayerController* PC = GetWorld()->GetFirstPlayerController();
				AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
				PS->AbilitySystemComponent->OnPeriodicGameplayEffectExecuteDelegateOnSelf.RemoveAll(CurrentWoodcuttingAbilityInstance);
				UE_LOG(LogTemp, Warning, TEXT("CurrentWoodcuttingAbilityInstance removed periodicgameplayeffectdelegate in playercontroller"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("CurrentWoodcuttingAbilityInstance is null ptr in PlayerController"));
			}
			CurrentTree->SetLifeSpan(-1.0f);
			//UE_LOG(LogTemp, Warning, TEXT("Tree life span: %f"), CurrentTree->GetLifeSpan());
		}
	}
}

void AIdlePlayerController::HandleZoomAction(const FInputActionValue& InputActionValue)
{
	//UE_LOG(LogTemp, Warning, TEXT("Zoom action called"));
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		if (AIdleCharacter* MyCharacter = Cast<AIdleCharacter>(ControlledPawn))
		{
			float CurrentArmLength = MyCharacter->SpringArm->TargetArmLength;

			// Modify this zoom speed as desired
			float ZoomSpeed = 100.0f;
			float InputValue = InputActionValue.Get<float>();
			float NewArmLength = FMath::Clamp(CurrentArmLength - InputValue * ZoomSpeed, MinZoomDistance, MaxZoomDistance);

			MyCharacter->SpringArm->TargetArmLength = NewArmLength;  // Changed this to modify the SpringArm's TargetArmLength
		}
	}
}


void AIdlePlayerController::MoveToClickLocation(const FInputActionValue& InputActionValue, FHitResult CursorHit, APawn* PlayerPawn)
{
	TargetDestination = CursorHit.ImpactPoint;

	// Start moving towards the target using the NavMesh
	if (UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this))
	{
		if (PlayerPawn)
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, TargetDestination);
		}
	}
}

void AIdlePlayerController::InterruptTreeCutting()
{
	//UE_LOG(LogTemp, Warning, TEXT("Interrupt Tree Cutting"));
	bIsChoppingTree = false;

	AIdleCharacter* MyCharacter = Cast<AIdleCharacter>(GetCharacter());
	if (MyCharacter && WoodcuttingEffectHandle.IsValid())
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(WoodcuttingEffectHandle);
		WoodcuttingEffectHandle.Invalidate();
		//UE_LOG(LogTemp, Warning, TEXT("WoodcuttingEffectHandle Invalidated"));
		UAnimMontage* AnimMontage = MyCharacter->WoodcutMontage;
		MyCharacter->StopAnimMontage(AnimMontage);
		EndTreeCutEffect();
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Character or WoodcuttingEffectHandle is null player controller"));
	}
}

void AIdlePlayerController::ClickTree(FHitResult TreeHit, APawn* PlayerPawn)
{
	//UE_LOG(LogTemp, Warning, TEXT("ClickTree"));
	if (bIsChoppingTree)
	{
		return;
	}

	TargetTree = TreeHit.GetActor();
	bIsMovingToTarget = true;

	// Start moving towards the tree using the NavMesh
	if (PlayerPawn)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, TargetTree->GetActorLocation());
	}
}

void AIdlePlayerController::OnCofferClicked(FHitResult CofferHit, APawn* PlayerPawn)
{
	//UE_LOG(LogTemp, Warning, TEXT("CofferClicked"));
	TargetCoffer = CofferHit.GetActor();
	CofferHitForCasting = CofferHit;
	bIsMovingToCoffer = true;
	ClickedCoffer = Cast<ACoffer>(CofferHit.GetActor());
	if (PlayerPawn)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, TargetCoffer->GetActorLocation());
	}
}


void AIdlePlayerController::ResetTreeTimer(AIdleEffectActor* Tree)
{
	//UE_LOG(LogTemp, Warning, TEXT("ResetTreeTimer"));
	AIdleActorManager* ActorManager = AIdleActorManager::GetInstance(GetWorld());
	if (ActorManager)
	{
		FName TreeName = Tree->GetFName(); // Convert the tree pointer to its name.
		//UE_LOG(LogTemp, Warning, TEXT("Trying to reset timer for tree: %s"), *TreeName.ToString());

		if (ActorManager->TreeTimers.Contains(TreeName))
		{
			ActorManager->TreeChoppingStates[TreeName] = false;
			FTimerHandle TreeTimerHandle = ActorManager->TreeTimers[TreeName];

			// Print the remaining time for the timer before resetting
			float TimeRemainingBeforeReset = GetWorld()->GetTimerManager().GetTimerRemaining(TreeTimerHandle);
			//UE_LOG(LogTemp, Warning, TEXT("Time remaining before reset: %f"), TimeRemainingBeforeReset);

			GetWorld()->GetTimerManager().ClearTimer(TreeTimerHandle);
			ActorManager->TreeTimers.Remove(TreeName);
			APlayerController* PC = GetWorld()->GetFirstPlayerController();
			AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
			//PS->DeactivateAbility(CurrentWoodcuttingAbilityInstance->StaticClass());

			// Print the remaining time for the timer after resetting
			float TimeRemainingAfterReset = GetWorld()->GetTimerManager().GetTimerRemaining(TreeTimerHandle);
			//UE_LOG(LogTemp, Warning, TEXT("Time remaining after reset: %f"), TimeRemainingAfterReset);

			//UE_LOG(LogTemp, Warning, TEXT("ResetTreeTimer After"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Tree Timer null/doesn't contain tree name"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ActorManager is a null ptr"));
	}
}

void AIdlePlayerController::ResetWoodcuttingAbilityTimer()
{
	if (CurrentWoodcuttingAbilityInstance)
	{
		CurrentWoodcuttingAbilityInstance->StopCutDownTimer();
		//UE_LOG(LogTemp, Warning, TEXT("Stop cut down timer"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not stop cutdowntimer CWA null"));
	}
}

void AIdlePlayerController::StartWoodcuttingAbility(APawn* PlayerPawn)
{
	bIsMovingToTarget = false;
	//UE_LOG(LogTemp, Warning, TEXT("Start woodcutting ability"));

	// Step 1: Calculate direction to the tree from the character's current location
	FVector DirectionToTree = (TargetTree->GetActorLocation() - PlayerPawn->GetActorLocation()).GetSafeNormal();
	FRotator RotationTowardsTree = DirectionToTree.Rotation();

	// Preserve the current Pitch and Roll of the character
	RotationTowardsTree.Pitch = PlayerPawn->GetActorRotation().Pitch;
	RotationTowardsTree.Roll = PlayerPawn->GetActorRotation().Roll;

	// Step 2: Set the character's rotation to face that direction
	PlayerPawn->SetActorRotation(RotationTowardsTree);

	AIdleCharacter* MyCharacter = Cast<AIdleCharacter>(PlayerPawn);
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();

	

	if (MyCharacter->EssenceCount <= 24)
	{
		bIsChoppingTree = true;

	
	    //PS->ActivateAbility(CurrentWoodcuttingAbilityInstance->StaticClass());
		PS->ActivateAbility(UWoodcuttingAbility::StaticClass());
		//CurrentWoodcuttingAbilityInstance->ActivateAbility();
		//UE_LOG(LogTemp, Warning, TEXT("ActivateAbility pc"));
		

		WoodcuttingEXPEffect();

		AIdleEffectActor* MyIdleEffectActor = Cast<AIdleEffectActor>(TargetTree);
		if (MyIdleEffectActor)
		{
			// Unbind first to ensure no multiple bindings
			OnTreeClicked.RemoveDynamic(MyIdleEffectActor, &AIdleEffectActor::SetTreeLifespan);
			OnTreeClicked.AddDynamic(MyIdleEffectActor, &AIdleEffectActor::SetTreeLifespan);
			OnTreeClicked.Broadcast(MyIdleEffectActor);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Cast to AIdleEffectActor failed! player controller"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory is full in player controller!"));
	}
}

void AIdlePlayerController::StartConversionAbility(APawn* PlayerPawn)
{
	bIsMovingToTarget = false;
	bIsChoppingTree = false;
	bHasPerformedCofferClick = true;
	bIsMovingToCoffer = false;

	// Step 1: Calculate direction to the tree from the character's current location
	FVector DirectionToCoffer = (TargetCoffer->GetActorLocation() - PlayerPawn->GetActorLocation()).GetSafeNormal();
	FRotator RotationTowardsCoffer = DirectionToCoffer.Rotation();

	// Preserve the current Pitch and Roll of the character
	RotationTowardsCoffer.Pitch = PlayerPawn->GetActorRotation().Pitch;
	RotationTowardsCoffer.Roll = PlayerPawn->GetActorRotation().Roll;

	// Step 2: Set the character's rotation to face that direction
	PlayerPawn->SetActorRotation(RotationTowardsCoffer);

	/*
	FVector DirectionToCoffer = (TargetCoffer->GetActorLocation() - PlayerPawn->GetActorLocation()).GetSafeNormal();
	FRotator RotationTowardsCoffer = DirectionToCoffer.Rotation();

	// Step 2: Set the character's rotation to face that direction
	PlayerPawn->SetActorRotation(RotationTowardsCoffer);

	*/

	ClickedCoffer->CofferClicked(CofferHitForCasting);
	
}


void AIdlePlayerController::WoodcuttingEXPEffect()
{
	//UE_LOG(LogTemp, Warning, TEXT("Woodcutting EXP Effect"));
	FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
	const FGameplayEffectSpecHandle EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(WoodcuttingGameplayEffect, 1.f, EffectContextHandle);
	WoodcuttingEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}

void AIdlePlayerController::SpawnTreeCutEffect()
{
	AIdleCharacter* ParticleCharacter = Cast<AIdleCharacter>(GetCharacter());
	if (!ParticleCharacter) return;

	USkeletalMeshComponent* CharacterWeapon = ParticleCharacter->GetMesh();
	StaffEndLocation = CharacterWeapon->GetSocketLocation(FName("StaffEndSocket"));
	FVector TreeLocation = CurrentTree->GetActorLocation();
	//StaffEndLocation.Z -= ZMultiplierStaffEndLoc;
	//StaffEndLocation.X += XMultiplierStaffEndLoc;
	//StaffEndLocation.Y += YMultiplierStaffEndLoc;
	TreeLocation.Z += ZMultiplierTreeLoc;
	
	

	FRotator RotationTowardsTree = UKismetMathLibrary::FindLookAtRotation(StaffEndLocation, TreeLocation);
	RotationTowardsTree.Yaw += YawRotationStaffMultiplier; //Move's the pitch downward slightly
	RotationTowardsTree.Pitch += PitchRotationStaffMultiplier;
	RotationTowardsTree.Roll += RollRotationStaffMultiplier;

	// Spawn the Niagara effects
	SpawnedTreeEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TreeCutEffect, CurrentTree->GetActorLocation());
	SpawnedStaffEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), StaffEffect, StaffEndLocation, RotationTowardsTree);
	//SpawnedStaffEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), StaffEffect, StaffEndLocation, RotationTowardsTree);

	// Draw a debug directional arrow
	//DrawDebugDirectionalArrow(GetWorld(), StaffEndLocation, StaffEndLocation + RotationTowardsTree.Vector() * 200, 20, FColor::Red, true, 5.0f, 0, 3.0f);
}

void AIdlePlayerController::EndTreeCutEffect()
{
	if (SpawnedTreeEffect && SpawnedStaffEffect)
	{
		SpawnedTreeEffect->Deactivate();
		SpawnedStaffEffect->Deactivate();
	}
}
