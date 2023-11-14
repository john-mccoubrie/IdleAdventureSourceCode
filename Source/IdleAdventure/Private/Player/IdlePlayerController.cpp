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
#include "Styling/SlateColor.h"
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
#include <Chat/GameChatManager.h>
#include <Actor/CofferManager.h>

AIdlePlayerController::AIdlePlayerController()
{
	bReplicates = true;

	IdleInteractionComponent = CreateDefaultSubobject<UIdleInteractionComponent>(TEXT("TreeInteractionComponent"));

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableObj(TEXT("/Game/Blueprints/DataTables/DT_PlayerDefaults.DT_PlayerDefaults"));
	if (DataTableObj.Succeeded())
	{
		PlayerDefaultsTable = DataTableObj.Object;
	}

	// Access the data:
	if (PlayerDefaultsTable)
	{
		FPlayerControllerDefaults* Defaults = PlayerDefaultsTable->FindRow<FPlayerControllerDefaults>(FName("PlayerDefaultsRow"), TEXT(""));
		if (Defaults)
		{
			WoodcuttingCastingDistance = Defaults->WoodcuttingCastingDistance;
			CofferCastingDistance = Defaults->CofferCastingDistance;
			ZMultiplierStaffEndLoc = Defaults->ZMultiplierStaffEndLoc;
			XMultiplierStaffEndLoc = Defaults->XMultiplierStaffEndLoc;
			YMultiplierStaffEndLoc = Defaults->YMultiplierStaffEndLoc;
			ZMultiplierTreeLoc = Defaults->ZMultiplierTreeLoc;
			MinZoomDistance = Defaults->MinZoomDistance;
			MaxZoomDistance = Defaults->MaxZoomDistance;
			NPCTalkingDistance = Defaults->NPCTalkingDistance;
		}
	}

	CurrentPlayerState = EPlayerState::Idle;
}

void AIdlePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	switch (CurrentPlayerState)
	{
	case EPlayerState::MovingToTree:
		if (TargetTree)
		{
			MoveTowardsTarget(TargetTree, WoodcuttingCastingDistance, [this](APawn* PlayerPawn) {
				StartWoodcuttingAbility(PlayerPawn);
				return;
				});
		}
		break;

	case EPlayerState::MovingToCoffer:
		if (TargetCoffer)
		{
			MoveTowardsTarget(TargetCoffer, CofferCastingDistance, [this](APawn* PlayerPawn) {
				StartConversionAbility(PlayerPawn);
				return;
				});
		}
		break;

	case EPlayerState::MovingToNPC:
		if (TargetNPC)
		{
			MoveTowardsTarget(TargetNPC, NPCTalkingDistance, [this](APawn* PlayerPawn) {
				StartNPCInteraction(PlayerPawn);
				return;
				});
		}
		break;

	default:
		break;
	}
}

void AIdlePlayerController::MoveTowardsTarget(AActor* Target, float CastingDistance, TFunction<void(APawn*)> OnReachTarget)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("ControlledPawn is null"));
		return;
	}

	FVector CurrentLocation = ControlledPawn->GetActorLocation();
	FVector PawnLocation2D = FVector(CurrentLocation.X, CurrentLocation.Y, 0);
	FVector TargetLocation2D = FVector(Target->GetActorLocation().X, Target->GetActorLocation().Y, 0);

	if (FVector::Distance(PawnLocation2D, TargetLocation2D) <= CastingDistance)
	{
		OnReachTarget(ControlledPawn);
	}
}

void AIdlePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeActorManager();
    SetupPlayerState();
    SetupAbilitySystemComponent();
    InitializeWoodcuttingAbility();
    InitializeConversionAbility();
    SetupInputSubsystem();
    ConfigureMouseCursor();
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

void AIdlePlayerController::InitializeActorManager()
{
	AIdleActorManager::GetInstance(GetWorld());
}

void AIdlePlayerController::SetupPlayerState()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
	AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PS);
}

void AIdlePlayerController::SetupAbilitySystemComponent()
{
	if (!CurrentWoodcuttingAbilityInstance)
	{
		CurrentWoodcuttingAbilityInstance = NewObject<UWoodcuttingAbility>(this, UWoodcuttingAbility::StaticClass());
	}
}

void AIdlePlayerController::InitializeWoodcuttingAbility()
{
	FGameplayAbilityInfo WoodcuttingAbilityInfo;
	WoodcuttingAbilityInfo.Ability = UWoodcuttingAbility::StaticClass();
	WoodcuttingAbilityInfo.Level = 1;
	WoodcuttingAbilityInfo.InputID = 0;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
	PS->GiveAbility(WoodcuttingAbilityInfo);
}

void AIdlePlayerController::InitializeConversionAbility()
{
	FGameplayAbilityInfo ConversionAbilityInfo;
	ConversionAbilityInfo.Ability = UConversionAbility::StaticClass();
	ConversionAbilityInfo.Level = 1;
	ConversionAbilityInfo.InputID = 0;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();
	PS->GiveAbility(ConversionAbilityInfo);
}

void AIdlePlayerController::SetupInputSubsystem()
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(IdleContext, 0);
	}
}

void AIdlePlayerController::ConfigureMouseCursor()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
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
	FHitResult ClickResult;
	GetHitResultUnderCursor(ECC_Visibility, false, ClickResult);
	APawn* PlayerPawn = GetPawn<APawn>();


	if (ClickResult.GetComponent()->ComponentTags.Contains("Tree"))
	{
		TreeClickEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TreeClickEffectSystem, ClickResult.Location);
		if (CurrentPlayerState == EPlayerState::CuttingTree)
		{
			return;
		}
		CurrentPlayerState = EPlayerState::MovingToTree;
		TargetTree = Cast<AIdleEffectActor>(ClickResult.GetActor());
		CurrentTree = Cast<AIdleEffectActor>(ClickResult.GetActor());
		ClickTree(ClickResult, PlayerPawn);
	}
	else if (ClickResult.GetComponent()->ComponentTags.Contains("Coffer"))
	{
		CofferClickEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CofferClickEffectSystem, ClickResult.Location);
		AIdleCharacter* MyCharacter = Cast<AIdleCharacter>(GetCharacter());
		if (MyCharacter->EssenceCount == 0)
		{
			AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
			GameChatManager->PostNotificationToUI(TEXT("You have no essence to add to the coffer."), FLinearColor::Red);
			return;
		}
		if (CurrentPlayerState == EPlayerState::CuttingTree)
		{
			InterruptTreeCutting();
		}
		CurrentPlayerState = EPlayerState::MovingToCoffer;
		OnCofferClicked(ClickResult, PlayerPawn);	
	}
	else if (ClickResult.GetComponent()->ComponentTags.Contains("NPC"))
	{
		CofferClickEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CofferClickEffectSystem, ClickResult.Location);
		if (CurrentPlayerState == EPlayerState::CuttingTree)
		{
			InterruptTreeCutting();
		}
		CurrentPlayerState = EPlayerState::MovingToNPC;
		TargetNPC = Cast<ANPCActor>(ClickResult.GetActor());
		MoveToClickLocation(InputActionValue, ClickResult, PlayerPawn);
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Clicked on the ground"));
		if (CurrentPlayerState == EPlayerState::CuttingTree)
		{
			InterruptTreeCutting();
		}
		//CurrentPlayerState = EPlayerState::Idle;
		MouseClickEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MouseClickEffectSystem, ClickResult.Location);
		MoveToClickLocation(InputActionValue, ClickResult, PlayerPawn);
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
			float ZoomSpeed = 300.0f;
			float InputValue = InputActionValue.Get<float>();
			float NewArmLength = FMath::Clamp(CurrentArmLength - InputValue * ZoomSpeed, MinZoomDistance, MaxZoomDistance);

			MyCharacter->SpringArm->TargetArmLength = NewArmLength;  // Changed this to modify the SpringArm's TargetArmLength
		}
	}
}


void AIdlePlayerController::MoveToClickLocation(const FInputActionValue& InputActionValue, FHitResult CursorHit, APawn* PlayerPawn)
{
	//UE_LOG(LogTemp, Warning, TEXT("Clicked on the ground"));
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
	UE_LOG(LogTemp, Warning, TEXT("Interrupt Tree Cutting"));
	CurrentPlayerState = EPlayerState::Idle;


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
		//UE_LOG(LogTemp, Warning, TEXT("CurrentWoodcuttingAbilityInstance removed periodicgameplayeffectdelegate in playercontroller"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentWoodcuttingAbilityInstance is null ptr in PlayerController"));
	}
	CurrentTree->SetLifeSpan(-1.0f);


	AIdleCharacter* MyCharacter = Cast<AIdleCharacter>(GetCharacter());
	if (MyCharacter && WoodcuttingEffectHandle.IsValid())
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(WoodcuttingEffectHandle);
		WoodcuttingEffectHandle.Invalidate();
		//UE_LOG(LogTemp, Warning, TEXT("WoodcuttingEffectHandle Invalidated"));
		UAnimMontage* AnimMontage = MyCharacter->WoodcutMontage;
		MyCharacter->StopAnimMontage(AnimMontage);
		IdleInteractionComponent->EndTreeCutEffect();
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Character or WoodcuttingEffectHandle is null player controller"));
	}
}

void AIdlePlayerController::ClickTree(FHitResult TreeHit, APawn* PlayerPawn)
{
	if (IdleInteractionComponent)
	{
		IdleInteractionComponent->ClickTree(TreeHit, PlayerPawn);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("IdleInteractionComponent is null"));
	}

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
	ClickedCoffer = Cast<ACoffer>(CofferHit.GetActor());
	if (PlayerPawn)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, TargetCoffer->GetActorLocation());
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
	CurrentPlayerState = EPlayerState::CuttingTree;

	AIdleCharacter* MyCharacter = Cast<AIdleCharacter>(PlayerPawn);
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	AIdlePlayerState* PS = PC->GetPlayerState<AIdlePlayerState>();

	if (MyCharacter->EssenceCount <= 24)
	{
		IdleInteractionComponent->StartWoodcuttingAbility(PlayerPawn);
		WoodcuttingEXPEffect();
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Inventory is full in player controller!"));
		AGameChatManager* GameChatManager = AGameChatManager::GetInstance(GetWorld());
		GameChatManager->PostNotificationToUI(TEXT("Inventory is full! Add your essence to the nearest coffer."), FLinearColor::Red);
	}
}

void AIdlePlayerController::StartConversionAbility(APawn* PlayerPawn)
{
	CurrentPlayerState = EPlayerState::Idle;

	// Step 1: Calculate direction to the tree from the character's current location
	FVector DirectionToCoffer = (TargetCoffer->GetActorLocation() - PlayerPawn->GetActorLocation()).GetSafeNormal();
	FRotator RotationTowardsCoffer = DirectionToCoffer.Rotation();

	// Preserve the current Pitch and Roll of the character
	RotationTowardsCoffer.Pitch = PlayerPawn->GetActorRotation().Pitch;
	RotationTowardsCoffer.Roll = PlayerPawn->GetActorRotation().Roll;

	// Step 2: Set the character's rotation to face that direction
	PlayerPawn->SetActorRotation(RotationTowardsCoffer);


	ClickedCoffer->CofferClicked(CofferHitForCasting);
	
}

void AIdlePlayerController::StartNPCInteraction(APawn* PlayerPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("StartNPCInteraction in PlayerController called."));
	CurrentPlayerState = EPlayerState::Idle;

	TargetNPC->Interact();
}


void AIdlePlayerController::WoodcuttingEXPEffect()
{
	//UE_LOG(LogTemp, Warning, TEXT("Woodcutting EXP Effect"));
	FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
	const FGameplayEffectSpecHandle EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(WoodcuttingGameplayEffect, 1.f, EffectContextHandle);
	WoodcuttingEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}

