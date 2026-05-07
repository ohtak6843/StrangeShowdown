// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/STLobbyLocalPlayer.h"
#include "Character/Player/STLobbyFieldPlayer.h"
#include "Game/STGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Controller/STLobbyController.h"
#include "Widget/STLobbyHUD.h"

#include "Game/STGameInstance.h"
#include "Manager/STDataManager.h"

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

void ASTLobbyLocalPlayer::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC)
		return;

	// LobbyController로 캐스팅
	ASTLobbyController* STPC = Cast<ASTLobbyController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (STPC)
	{
		LobbyHUDWidget = STPC->LobbyHUDWidget;
		if (!LobbyHUDWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to get LobbyHUDWidget from Controller"));
			return;
		}
	}

	uint64 MyId{ 0 };

	// todo cham: 이름도 가져와야 함.
#if NETWORK_ENABLED
	
	auto* GameInstance{ Cast<USTGameInstance>(GetWorld()->GetGameInstance()) };
	auto* DataManager{ GameInstance ? GameInstance->GetDataManager() : nullptr };
	if (DataManager)
	{
		MyId = DataManager->GetMyPlayerInfo().PlayerID;
		PlayerNickName = DataManager->GetMyPlayerInfo().NickName;
	}

#endif


	AddPlayerInWidget(MyId, PlayerNickName, false);

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