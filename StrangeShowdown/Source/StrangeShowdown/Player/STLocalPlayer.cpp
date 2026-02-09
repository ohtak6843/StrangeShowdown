// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/STLocalPlayer.h"
#include "Item/STPickupItem.h"
#include "Component/STStoreComponent.h" 
#include "Component/STInventoryComponent.h"
#include "Component/STQuickSlotComponent.h"
#include "Game/NetworkGameInstance.h"
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
}

void ASTLocalPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void ASTLocalPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
#if NETWORK_ENABLED
	SendMovePacket(DeltaTime);
#endif
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
		Cast<UNetworkGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);
	}
}
