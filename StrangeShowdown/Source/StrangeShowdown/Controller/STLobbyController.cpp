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

void ASTLobbyController::InitData(const uint64 InHostPlayerID, const uint64 InMyPlayerID)
{
	// 초기 정보 설정
	HostPlayerID = InHostPlayerID;
	MyPlayerID = InMyPlayerID;
	bIsRoomOwner = (HostPlayerID == MyPlayerID);

	// 받아온 정보로 플레이어 갱신
	UpdateReadyText();
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
	
	// hud 갱신
	LobbyHUDWidget->SetReady(MyPlayerID, bIsReady);
	UE_LOG(LogTemp, Log, TEXT("Player %llu is %s"), MyPlayerID, bIsReady ? TEXT("ready") : TEXT("not ready"));
}

void ASTLobbyController::SetOtherPlayerReady(const uint64 InPlayerID, const bool InReady)
{
	LobbyHUDWidget->SetOtherPlayerReady(InPlayerID, InReady);
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