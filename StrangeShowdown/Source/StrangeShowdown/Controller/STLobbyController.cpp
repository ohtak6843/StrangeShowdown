// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/STLobbyController.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "Components/EditableTextBox.h"
#include "Widget/STLobbyHUD.h"

#include "Game/STGameInstance.h"

ASTLobbyController::ASTLobbyController()
{
	AddInputAction();
}

void ASTLobbyController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(ReadyAction, ETriggerEvent::Started, this, &ASTLobbyController::SetReady);
	EnhancedInputComponent->BindAction(FocusChatManagerAction, ETriggerEvent::Triggered, this, &ASTLobbyController::FocusChatManager);
}

USTChatManagerWidget* ASTLobbyController::GetChatManagerWidget()
{
	ISTControllerHUDInterface::GetChatManagerWidget();
	return LobbyHUDWidget->ChatManagerWidget;
}

void ASTLobbyController::BeginPlay()
{
	Super::BeginPlay();

	// TODO: 서버에서 현재 플레이어 수 받아오기
	currentPlayerCount = 5;

	UpdateReadyText();
}

void ASTLobbyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASTLobbyController::SetReady()
{
	if (bIsRoomOwner)
	{
		// 방장일 경우 start
#if NETWORK_ENABLED

		GetGameInstance<USTGameInstance>()->StartGame();

#endif // NETWORK_ENABLED
		return;
	}
	bIsReady = !bIsReady;

#if NETWORK_ENABLED

	GetGameInstance<USTGameInstance>()->Ready(bIsReady);

#endif // NETWORK_ENABLED
	
	// todo cham: 1 대신 PlayerID로 슬롯 업데이트
	// 본인의 id를 알려주는 것이 필요.
	LobbyHUDWidget->LobbyStatusWidget->SetPlayerReady(0, bIsReady);
	LobbyHUDWidget->ReadyText->SetVisibility(!bIsReady ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

}

void ASTLobbyController::SetReady(const uint64 InPlayerID, const bool InReady)
{
	LobbyHUDWidget->LobbyStatusWidget->SetPlayerReady(InPlayerID, InReady);
}

void ASTLobbyController::UpdateReadyText()
{
	if (!bIsRoomOwner) return;

	if (currentPlayerCount == maxPlayerCount)
	{
		LobbyHUDWidget->ReadyText->SetText(FText::FromString(TEXT("Press space to start")));
	}
	else
	{
		LobbyHUDWidget->ReadyText->SetText(FText::FromString(TEXT("Waiting for players... ( ") + FString::FromInt(currentPlayerCount) + TEXT("/") + FString::FromInt(maxPlayerCount) + TEXT(" )")));
	}
}

void ASTLobbyController::AddInputAction()
{
	// Input Actions
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionReadyRef(TEXT("/Script/EnhancedInput.InputAction'/Game/StrangeShowdown/Input/Actions/IA_Ready.IA_Ready'"));
	if (nullptr != InputActionReadyRef.Object)
	{
		ReadyAction = InputActionReadyRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionFocusChatManagerRef(TEXT("/Script/EnhancedInput.InputAction'/Game/StrangeShowdown/Input/Actions/IA_FocusChatManager.IA_FocusChatManager'"));
	if (nullptr != InputActionFocusChatManagerRef.Object)
	{
		FocusChatManagerAction = InputActionFocusChatManagerRef.Object;
	}
}

void ASTLobbyController::FocusChatManager()
{
	SetInputMode(FInputModeGameAndUI());

	LobbyHUDWidget->ChatManagerWidget->GetChatInputTextBox()->SetKeyboardFocus();
}