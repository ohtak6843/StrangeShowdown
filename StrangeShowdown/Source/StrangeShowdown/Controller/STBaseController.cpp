// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/STBaseController.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "Actor/STInteractableActor.h"
#include "Character/Player/STLocalPlayer.h"

ASTBaseController::ASTBaseController()
{
	AddInputAction();
}

void ASTBaseController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ASTBaseController::Interact);
}

void ASTBaseController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}

void ASTBaseController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASTBaseController::Interact()
{
	UWorld* World = GetWorld();
	if (!World)
		return;

	FVector CameraLocation;
	FRotator CameraRotation;
	GetPlayerViewPoint(CameraLocation, CameraRotation);

	const FVector Start = CameraLocation;
	const FVector End = Start + CameraRotation.Vector() * 2000.f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetPawn());

	FHitResult Hit;

	bool bHit = World->SweepSingleByChannel(
		Hit,
		Start,
		End,
		FQuat::Identity,
		ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(100.f),
		Params
	);

	if (bHit)
	{
		AActor* HitActor = Hit.GetActor();

		if (HitActor && HitActor->Implements<UInteractable>())
		{
			// 해당 액터에 맞는 Interact 함수 호출
			IInteractable::Execute_Interact(HitActor, GetPawn());
		}
	}
}

void ASTBaseController::AddInputAction()
{
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionFocusChatManagerRef(TEXT("/Script/EnhancedInput.InputAction'/Game/StrangeShowdown/Input/Actions/IA_Interact.IA_Interact'"));
	if (nullptr != InputActionFocusChatManagerRef.Object)
	{
		InteractAction = InputActionFocusChatManagerRef.Object;
	}
}