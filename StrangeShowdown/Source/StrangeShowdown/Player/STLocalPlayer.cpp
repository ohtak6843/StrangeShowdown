// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/STLocalPlayer.h"
#include "Item/STPickupItem.h"
#include "Component/STStoreComponent.h" 
#include "Component/STInventoryComponent.h"
#include "Component/STQuickSlotComponent.h"
#include "Game/STGameInstance.h"
#include "StrangeShowdown.h"

ASTLocalPlayer::ASTLocalPlayer()
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

	// Inventory Component
	InventoryComp = CreateDefaultSubobject<USTInventoryComponent>(TEXT("InventoryComp"));

	// Store Component
	StoreComp = CreateDefaultSubobject<USTStoreComponent>(TEXT("StoreComp"));

	// Attack Trace Component
	AttackTraceComp = CreateDefaultSubobject<USTAttackTraceComponent>(TEXT("AttackTraceComp"));

	// Camera Pose Settings
	PoseSettings.Add(ECameraPose::Idle, FCameraPoseSetting{ 300.f, 0.f });
	PoseSettings.Add(ECameraPose::Aiming, FCameraPoseSetting{ 100.f, 70.f });
	PoseSettings.Add(ECameraPose::LookingUp, FCameraPoseSetting{ 200.f, 40.f });
}

void ASTLocalPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void ASTLocalPlayer::Tick(float DeltaTime)
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

void ASTLocalPlayer::SetCameraPose(ECameraPose NewPose)
{
	StartPose.SpringArmLength = SpringArmComp->TargetArmLength;
	StartPose.CameraY = CameraComp->GetRelativeLocation().Y;

	TargetPose = PoseSettings[NewPose];

	PoseElapsedTime = 0.f;
}

void ASTLocalPlayer::ApplyStateSettings(ECameraPose NewState)
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

void ASTLocalPlayer::Interact(int32& OutAddedInventoryIndex)
{
	FVector Start = CameraComp->GetComponentLocation();
	FVector ForwardVector = CameraComp->GetForwardVector();
	FVector End = ((ForwardVector * 500.f) + Start);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;

	bool bIsHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		CollisionParams
	);

	if (!bIsHit)
		return;

	ASTPickupItem* PickupItem = Cast<ASTPickupItem>(HitResult.GetActor());
	if (!PickupItem || !InventoryComp || !PickupItem->ItemData)
		return;

	int32 AddedInventoryIndex = -1;

	bool bAdded = InventoryComp->AddItem(
		PickupItem->ItemData,
		1,
		AddedInventoryIndex
	);

	// 만약 퀵슬롯에 추가한 아이템이 이미 있다면 퀵슬롯의 count도 증가
	USTQuickSlotComponent* QuickSlotComp = InventoryComp->GetOwner()->FindComponentByClass<USTQuickSlotComponent>();
	if (bAdded && QuickSlotComp)
	{
		for (int32 i = 0; i < QuickSlotComp->QuickSlots.Num(); i++)
		{
			FInventorySlot& QuickSlot = QuickSlotComp->QuickSlots[i];
			if (QuickSlot.ItemData == PickupItem->ItemData)
			{
				QuickSlot.Count += 1;
				QuickSlotComp->OnQuickSlotUpdated.Broadcast();
				break;
			}
		}
	}

	if (!bAdded || AddedInventoryIndex == -1)
		return;
	
	// AddInventoryIndex 반환
	OutAddedInventoryIndex = AddedInventoryIndex;

	PickupItem->Destroy();
}

void ASTLocalPlayer::ChangeToIdle()
{
	RemoveState(EPlayerStateFlag::Aiming);
	RemoveState(EPlayerStateFlag::LookingUp);
	bUseControllerRotationYaw = false;

	SetCameraPose(ECameraPose::Idle);
}

void ASTLocalPlayer::ChangeToAiming()
{
	AddState(EPlayerStateFlag::Aiming);
	RemoveState(EPlayerStateFlag::LookingUp);
	bUseControllerRotationYaw = true;

	SetCameraPose(ECameraPose::Aiming);
}

void ASTLocalPlayer::ChangeToLookingUp()
{
	AddState(EPlayerStateFlag::LookingUp);
	RemoveState(EPlayerStateFlag::Aiming);
	bUseControllerRotationYaw = true;

	SetCameraPose(ECameraPose::LookingUp);
}

void ASTLocalPlayer::SendMovePacket(const float DeltaTime)
{
	SendMoveDeltaTime += DeltaTime;

	if (SendMoveDeltaTime >= SendMoveMaxTime)
	{
		SendMoveDeltaTime -= SendMoveMaxTime;

		TArray<uint8> SendBuffer;
		auto rotation{ GetActorRotation() };
		packet::CSMovePlayer move_packet{
			Vec3f{
				static_cast<float>(GetActorLocation().X),
				static_cast<float>(GetActorLocation().Y),
				static_cast<float>(GetActorLocation().Z)
			},
			Vec3f{
				static_cast<float>(rotation.Pitch),
				static_cast<float>(rotation.Yaw),
				static_cast<float>(rotation.Roll)
			}
		};
		SendBuffer.AddUninitialized(move_packet.size);
		FMemory::Memcpy(SendBuffer.GetData(), &move_packet, move_packet.size);
		Cast<USTGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);
	}
}
