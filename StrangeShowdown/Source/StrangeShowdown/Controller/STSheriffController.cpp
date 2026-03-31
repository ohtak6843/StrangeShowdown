// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/STSheriffController.h"
#include "STSheriffController.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "UI/STHUDWidget.h"

ASTSheriffController::ASTSheriffController()
{
	AddInputAction();
}

void ASTSheriffController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(TabUIAction, ETriggerEvent::Triggered, this, &ASTSheriffController::ShowTabUI);
	EnhancedInputComponent->BindAction(BigMapAction, ETriggerEvent::Triggered, this, &ASTSheriffController::ShowBigMap);
	EnhancedInputComponent->BindAction(FocusChatManagerAction, ETriggerEvent::Triggered, this, &ASTSheriffController::FocusChatManager);
}

void ASTSheriffController::BeginPlay()
{
	Super::BeginPlay();
}

void ASTSheriffController::AddInputAction()
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

void ASTSheriffController::ShowTabUI()
{
	// TODO: RemoveAllUI가 C++로 구현 가능해지면 구현
}

void ASTSheriffController::ShowBigMap()
{
	// TODO: RemoveAllUI가 C++로 구현 가능해지면 구현
}

void ASTSheriffController::FocusChatManager()
{
	SetInputMode(FInputModeGameAndUI());

	// ChatManagerWidget 구현 후 활성화
	//HUDWidget->ChatManagerWidget->ChatInputTextBox->SetKeyboardFocus();
}