// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/STLocalPlayer.h"
#include "Item/STPickupItem.h"
#include "Component/STStoreComponent.h" 
#include "Component/STInventoryComponent.h"
#include "Component/STQuickSlotComponent.h"
#include "Component/STAttackTraceComponent.h"
#include "Item/STItemDataAssetBase.h"
#include "Game/STGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Actor/STMiniMapActor.h"
#include "Actor/STBigMapActor.h"
#include "StrangeShowdown.h"
#include "UI/STHUDWidget.h"
#include "UI/STQuickSlotWidget.h"

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

	// QuickSlot Component
	QuickSlotComp = CreateDefaultSubobject<USTQuickSlotComponent>(TEXT("QuickSlotComp"));

	// Store Component
	StoreComp = CreateDefaultSubobject<USTStoreComponent>(TEXT("StoreComp"));

	// Attack Trace Component
	AttackTraceComp = CreateDefaultSubobject<USTAttackTraceComponent>(TEXT("AttackTraceComp"));

	// Camera Pose Settings
	PoseSettings.Add(ECameraPose::Idle, FCameraPoseSetting{ 300.f, 0.f });
	PoseSettings.Add(ECameraPose::Aiming, FCameraPoseSetting{ 100.f, 70.f });
	PoseSettings.Add(ECameraPose::LookingUp, FCameraPoseSetting{ 200.f, 40.f });

	// MiniMap 검색
	TArray<AActor*> WorldMiniMapActors;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		ASTMiniMapActor::StaticClass(),
		WorldMiniMapActors);

	for (AActor* Actor : WorldMiniMapActors)
	{
		ASTMiniMapActor* MiniMap = Cast<ASTMiniMapActor>(Actor);
		if (MiniMap)
		{
			MiniMapActor = MiniMap;
			break;
		}
	}

	// BigMap 검색
	TArray<AActor*> WorldBigMapActors;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		ASTBigMapActor::StaticClass(),
		WorldBigMapActors);

	for (AActor* Actor : WorldBigMapActors)
	{
		ASTBigMapActor* BigMap = Cast<ASTBigMapActor>(Actor);
		if (BigMap)
		{
			BigMapActor = BigMap;
			break;
		}
	}
}

void ASTLocalPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Set Weapon and Hammer Data
	if (1 < QuickSlotComp->QuickSlots.Num())
	{
		USTItemDataAssetBase* PistolData = LoadObject<USTItemDataAssetBase>(nullptr, TEXT("/Game/StrangeShowdown/Item/DataAsset/DA_Pistol.DA_Pistol"));
		QuickSlotComp->QuickSlots[0].ItemData = PistolData;
		QuickSlotComp->QuickSlots[0].bIsCountable = false;

		USTItemDataAssetBase* HammerData = LoadObject<USTItemDataAssetBase>(nullptr, TEXT("/Game/StrangeShowdown/Item/DataAsset/DA_Hammer.DA_Hammer"));
		QuickSlotComp->QuickSlots[1].ItemData = HammerData;
		QuickSlotComp->QuickSlots[1].bIsCountable = false;
	}
}

void ASTLocalPlayer::BeginPlay()
{
	Super::BeginPlay();

	HoldItem();
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

void ASTLocalPlayer::SetupHUDWidget(USTHUDWidget* InHUDWidget)
{
	if (InHUDWidget)
	{
		InHUDWidget->ShowInventoryMenu(InventoryComp->Slots);
		InHUDWidget->UpdateInventoryMenu(InventoryComp->Slots);
		InHUDWidget->UpdateQuickSlots(QuickSlotComp->QuickSlots, QuickSlotComp->CurrentSelectQuickSlotIndex);

		InventoryComp->OnInventoryUpdated.AddUObject(InHUDWidget, &USTHUDWidget::UpdateInventoryMenu);
		QuickSlotComp->OnQuickSlotUpdated.AddUObject(InHUDWidget, &USTHUDWidget::UpdateQuickSlots);

		// OnDrop 시 업데이트 연결
		for (int i = 0; i < QuickSlotComp->QuickSlots.Num(); i++)
		{
			InHUDWidget->GetQuickSlotWidget(i)->OnQuickSlotWidgetDrop.AddUObject(QuickSlotComp, &USTQuickSlotComponent::AddItem);
		}
	}
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

	// TODO: 인벤토리 위젯 업데이트 하도록 브로드캐스트

	if (bAdded && QuickSlotComp)
	{
		for (int32 i = 0; i < QuickSlotComp->QuickSlots.Num(); i++)
		{
			FSTItemSlot& QuickSlot = QuickSlotComp->QuickSlots[i];
			if (QuickSlot.ItemData == PickupItem->ItemData)
			{
				QuickSlot.Count += 1;
				QuickSlotComp->OnQuickSlotUpdated.Broadcast(QuickSlotComp->QuickSlots, QuickSlotComp->CurrentSelectQuickSlotIndex);
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

void ASTLocalPlayer::HoldItem()
{
	USTItemDataAssetBase* ItemData = QuickSlotComp->GetCurrentSelectedQuickSlotItemData();
	if (!ItemData)
	{
		RightHandStaticMesh->SetStaticMesh(nullptr);
		RightHandSkeletalMesh->SetSkeletalMesh(nullptr);
		return;
	}

	switch (ItemData->ItemType)
	{
	case EItemType::Pistol:
		RightHandSkeletalMesh->SetSimulatePhysics(false);
		RightHandSkeletalMesh->SetEnableGravity(false);
		RightHandSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		RightHandSkeletalMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(*ItemData->ItemName.ToString()));
		RightHandSkeletalMesh->SetSkeletalMesh(ItemData->ItemSkeletalMesh);

		RightHandStaticMesh->SetStaticMesh(nullptr);
		break;

	case EItemType::Hammer:
	case EItemType::Helmet:
	case EItemType::Meat:
	case EItemType::Whiskey:
	case EItemType::EnhancePower:
		RightHandStaticMesh->SetSimulatePhysics(false);
		RightHandStaticMesh->SetEnableGravity(false);
		RightHandStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		RightHandStaticMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(*ItemData->ItemName.ToString()));
		RightHandStaticMesh->SetStaticMesh(ItemData->ItemStaticMesh);

		RightHandSkeletalMesh->SetSkeletalMesh(nullptr);
		break;
	}
}

void ASTLocalPlayer::DropItem()
{
	USTItemDataAssetBase* ItemData = QuickSlotComp->GetCurrentSelectedQuickSlotItemData();
	if (!ItemData) return;

	switch (ItemData->ItemType)
	{
	case EItemType::Pistol:
		RightHandSkeletalMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		RightHandSkeletalMesh->SetSimulatePhysics(true);
		RightHandSkeletalMesh->SetEnableGravity(true);
		RightHandSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;

	case EItemType::Hammer:
	case EItemType::Helmet:
	case EItemType::Meat:
	case EItemType::Whiskey:
	case EItemType::EnhancePower:
		RightHandStaticMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		RightHandStaticMesh->SetSimulatePhysics(true);
		RightHandStaticMesh->SetEnableGravity(true);
		RightHandStaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}

void ASTLocalPlayer::ChangeToIdle()
{
	RemoveState(EPlayerState::Aiming);
	RemoveState(EPlayerState::LookingUp);
	bUseControllerRotationYaw = false;

	SetCameraPose(ECameraPose::Idle);
}

void ASTLocalPlayer::ChangeToAiming()
{
	AddState(EPlayerState::Aiming);
	RemoveState(EPlayerState::LookingUp);
	bUseControllerRotationYaw = true;

	SetCameraPose(ECameraPose::Aiming);
}

void ASTLocalPlayer::ChangeToLookingUp()
{
	AddState(EPlayerState::LookingUp);
	RemoveState(EPlayerState::Aiming);
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
