// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/STGhostController.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "Widget/STHUD.h"

ASTGhostController::ASTGhostController()
{
	AddInputAction();
}

void ASTGhostController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(TabUIAction, ETriggerEvent::Triggered, this, &ASTGhostController::ShowTabUI);
	EnhancedInputComponent->BindAction(BigMapAction, ETriggerEvent::Triggered, this, &ASTGhostController::ShowBigMap);
	EnhancedInputComponent->BindAction(FocusChatManagerAction, ETriggerEvent::Triggered, this, &ASTGhostController::FocusChatManager);
}

void ASTGhostController::BeginPlay()
{
	Super::BeginPlay();
}

void ASTGhostController::AddInputAction()
{
	// Input Actions
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionTabUIRef(TEXT("/Script/EnhancedInput.InputAction'/Game/StrangeShowdown/Input/Actions/IA_TabUI.IA_TabUI'"));
	if (nullptr != InputActionTabUIRef.Object)
	{
		TabUIAction = InputActionTabUIRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionBigMapRef(TEXT("/Script/EnhancedInput.InputAction'/Game/StrangeShowdown/Input/Actions/IA_BigMap.IA_BigMap'"));
	if (nullptr != InputActionBigMapRef.Object)
	{
		BigMapAction = InputActionBigMapRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionFocusChatManagerRef(TEXT("/Script/EnhancedInput.InputAction'/Game/StrangeShowdown/Input/Actions/IA_FocusChatManager.IA_FocusChatManager'"));
	if (nullptr != InputActionFocusChatManagerRef.Object)
	{
		FocusChatManagerAction = InputActionFocusChatManagerRef.Object;
	}
}

void ASTGhostController::ShowTabUI()
{
	// TODO: RemoveAllUI가 C++로 구현 가능해지면 구현
}

void ASTGhostController::ShowBigMap()
{
	// TODO: RemoveAllUI가 C++로 구현 가능해지면 구현
}

void ASTGhostController::FocusChatManager()
{
	SetInputMode(FInputModeGameAndUI());

	HUDWidget->ChatManagerWidget->ChatInputTextBox->SetKeyboardFocus();
}