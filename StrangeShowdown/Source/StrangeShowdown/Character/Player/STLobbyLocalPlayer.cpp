// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/STLobbyLocalPlayer.h"
#include "Character/Player/STLobbyFieldPlayer.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Game/STGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Controller/STLobbyController.h"
#include "UI/Lobby/STLobbyHUD.h"

ASTLobbyLocalPlayer::ASTLobbyLocalPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	// Spring Arm Component
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	// Camera Component
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	// Camera Pose Settings
	PoseSettings.Add(ECameraPose::Idle, FCameraPoseSetting{ 300.f, 0.f });
	PoseSettings.Add(ECameraPose::Aiming, FCameraPoseSetting{ 100.f, 70.f });
	PoseSettings.Add(ECameraPose::LookingUp, FCameraPoseSetting{ 200.f, 40.f });
}

void ASTLobbyLocalPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &ASTLobbyLocalPlayer::ShoulderMove);
	EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &ASTLobbyLocalPlayer::ShoulderLook);
}

void ASTLobbyLocalPlayer::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// LobbyController로 캐스팅
	ASTLobbyController* STPC = Cast<ASTLobbyController>(PlayerController);
	if (STPC)
	{
		LobbyHUDWidget = STPC->LobbyHUDWidget;
		if (!LobbyHUDWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to get LobbyHUDWidget from Controller"));
			return;
		}
	}

	// 자기 자신 AddPlayerInWidget() 호출
	// TODO: 캐릭터도 ID를 가지고 있도록 해야함
	// todo cham: 내거야 
	AddPlayerInWidget(0, PlayerNickName, false);

	// Add Player 델리게이트
	ASTLobbyFieldPlayer::OnFieldPlayerSpawned.AddUObject(
		this,
		&ASTLobbyLocalPlayer::AddPlayerInWidget
	);

	// TODO: 기존 플레이어 가져와서 AddPlayerInWidget() 호출

	// Remove Player 델리게이트
	ASTLobbyFieldPlayer::OnFieldPlayerRemoved.AddUObject(
		this,
		&ASTLobbyLocalPlayer::RemovePlayerFromWidget
	);
}

void ASTLobbyLocalPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ASTLobbyFieldPlayer::OnFieldPlayerSpawned.RemoveAll(this);
	ASTLobbyFieldPlayer::OnFieldPlayerRemoved.RemoveAll(this);
}

void ASTLobbyLocalPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Camera Pose Blending
	PoseElapsedTime += DeltaTime;
	float Alpha = FMath::Clamp(PoseElapsedTime / PoseBlendTime, 0.f, 1.f);

	Alpha = FMath::InterpEaseInOut(0.f, 1.f, Alpha, 2.f);

	SpringArmComp->TargetArmLength = FMath::Lerp(StartPose.SpringArmLength, TargetPose.SpringArmLength, Alpha);

	FVector Rel = CameraComp->GetRelativeLocation();
	Rel.Y = FMath::Lerp(StartPose.CameraY, TargetPose.CameraY, Alpha);
	CameraComp->SetRelativeLocation(Rel);

	// Send Move Packet to Server
#if NETWORK_ENABLED
	SendMovePacket(DeltaTime);
#endif
}

void ASTLobbyLocalPlayer::SetupHUDWidget(USTHUDWidget* InHUDWidget)
{
	ISTCharacterHUDInterface::SetupHUDWidget(InHUDWidget);
}

void ASTLobbyLocalPlayer::ShoulderMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void ASTLobbyLocalPlayer::ShoulderLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ASTLobbyLocalPlayer::SetCameraPose(ECameraPose NewPose)
{
	StartPose.SpringArmLength = SpringArmComp->TargetArmLength;
	StartPose.CameraY = CameraComp->GetRelativeLocation().Y;

	TargetPose = PoseSettings[NewPose];

	PoseElapsedTime = 0.f;
}

void ASTLobbyLocalPlayer::ApplyStateSettings(ECameraPose NewState)
{
	switch (NewState)
	{
	case ECameraPose::Idle:
		ChangeToIdle();
		break;
	case ECameraPose::Aiming:
		ChangeToAiming();
		break;
	case ECameraPose::LookingUp:
		ChangeToLookingUp();
		break;
	default:
		break;
	}
}

void ASTLobbyLocalPlayer::ChangeToIdle()
{
	RemoveState(EPlayerState::Aiming);
	RemoveState(EPlayerState::LookingUp);
	bUseControllerRotationYaw = false;

	SetCameraPose(ECameraPose::Idle);
}

void ASTLobbyLocalPlayer::ChangeToAiming()
{
	AddState(EPlayerState::Aiming);
	RemoveState(EPlayerState::LookingUp);
	bUseControllerRotationYaw = true;

	SetCameraPose(ECameraPose::Aiming);
}

void ASTLobbyLocalPlayer::ChangeToLookingUp()
{
	AddState(EPlayerState::LookingUp);
	RemoveState(EPlayerState::Aiming);
	bUseControllerRotationYaw = true;

	SetCameraPose(ECameraPose::LookingUp);
}

void ASTLobbyLocalPlayer::SendMovePacket(const float DeltaTime)
{
	SendMoveDeltaTime += DeltaTime;

	if (SendMoveDeltaTime >= Common::SendMoveTime)
	{
		SendMoveDeltaTime -= Common::SendMoveTime;

		TArray<uint8> SendBuffer;
		auto rotation{ GetActorRotation() };
		Common::CSMovePlayer move_packet{
			Vec3f{
				static_cast<float>(GetActorLocation().X),
				static_cast<float>(GetActorLocation().Y),
				static_cast<float>(GetActorLocation().Z)
			},
			Vec3f{
				static_cast<float>(rotation.Pitch),
				static_cast<float>(rotation.Yaw),
				static_cast<float>(rotation.Roll)
			},
			PlayerStateFlag
		};

		SendBuffer.AddUninitialized(move_packet.size);
		FMemory::Memcpy(SendBuffer.GetData(), &move_packet, move_packet.size);
		Cast<USTGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);
	}
}

void ASTLobbyLocalPlayer::AddPlayerInWidget(uint64 PlayerID, const FString& NickName, bool bReady)
{
	if (LobbyHUDWidget)
	{
		LobbyHUDWidget->LobbyStatusWidget->EnterPlayer(PlayerID, NickName, bReady);
	}
}

void ASTLobbyLocalPlayer::RemovePlayerFromWidget(uint64 PlayerID)
{
	if (LobbyHUDWidget)
	{
		LobbyHUDWidget->LobbyStatusWidget->LeavePlayer(PlayerID);
	}
}