// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/STLocalPlayer.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Item/STPickupItem.h"
#include "Component/STStoreComponent.h" 
#include "Component/STInventoryComponent.h"
#include "Component/STQuickSlotComponent.h"
#include "Component/STAttackTraceComponent.h"
#include "Component/STMissionComponent.h"
#include "Item/STItemDataAssetBase.h"
#include "Game/STGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Actor/STMiniMapActor.h"
#include "Actor/STBigMapActor.h"
#include "Engine/DamageEvents.h"
#include "Actor/STSliceableActor.h"
#include "Particles/ParticleSystem.h"
#include "StrangeShowdown.h"
#include "UI/STHUDWidget.h"
#include "UI/Stat/STStatWidget.h"
#include "UI/Inventory/STInventoryMenuWidget.h"
#include "UI/QuickSlot/STQuickSlotWidget.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Protocol.h"

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

	// Stat Component
	StatComp->CurrentHp = StatComp->MaxHp;
	StatComp->Gold = 0;
	StatComp->Kill = 0;
	StatComp->CurrentArmor = 0;
	StatComp->MoveSpeed = 500;
	StatComp->CurrentStamina = StatComp->MaxStamina - 2;
	StatComp->CurrentAction = StatComp->UseAbleAction;
	StatComp->Prize = 0;
	StatComp->bAlive = true;

	// Inventory Component
	InventoryComp = CreateDefaultSubobject<USTInventoryComponent>(TEXT("InventoryComp"));

	// QuickSlot Component
	QuickSlotComp = CreateDefaultSubobject<USTQuickSlotComponent>(TEXT("QuickSlotComp"));

	// Store Component
	StoreComp = CreateDefaultSubobject<USTStoreComponent>(TEXT("StoreComp"));

	// Attack Trace Component
	AttackTraceComp = CreateDefaultSubobject<USTAttackTraceComponent>(TEXT("AttackTraceComp"));

	// Mission Component
	MissionComponent = CreateDefaultSubobject<USTMissionComponent>(TEXT("MissionComponent"));

	// Camera Pose Settings
	PoseSettings.Add(ECameraPose::Idle, FCameraPoseSetting{ 300.f, 0.f });
	PoseSettings.Add(ECameraPose::Aiming, FCameraPoseSetting{ 100.f, 70.f });
	PoseSettings.Add(ECameraPose::LookingUp, FCameraPoseSetting{ 200.f, 40.f });

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> EffectRef(TEXT("/Script/Niagara.NiagaraSystem'/Game/StrangeShowdown/Prop/SmashFX/NS_Smash.NS_Smash'"));
	if (EffectRef.Object)
	{
		HitEffect = EffectRef.Object;
	}

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
		// TODO: QuickSlot
		USTItemDataAssetBase* PistolData = LoadObject<USTItemDataAssetBase>(nullptr, TEXT("/Game/StrangeShowdown/Item/DataAsset/DA_Pistol.DA_Pistol"));
		QuickSlotComp->QuickSlots[0].ItemData = PistolData;
		QuickSlotComp->QuickSlots[0].bIsCountable = false;

		USTItemDataAssetBase* HammerData = LoadObject<USTItemDataAssetBase>(nullptr, TEXT("/Game/StrangeShowdown/Item/DataAsset/DA_Hammer.DA_Hammer"));
		QuickSlotComp->QuickSlots[1].ItemData = HammerData;
		QuickSlotComp->QuickSlots[1].bIsCountable = false;
	}
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

void ASTLocalPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &ASTLocalPlayer::ShoulderMove);
	EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &ASTLocalPlayer::ShoulderLook);
	EnhancedInputComponent->BindAction(PistolAimAction, ETriggerEvent::Triggered, this, &ASTLocalPlayer::PistolAim);
	EnhancedInputComponent->BindAction(PistolFireAction, ETriggerEvent::Triggered, this, &ASTLocalPlayer::PistolFire);
	EnhancedInputComponent->BindAction(ChangeQuickSlotAction, ETriggerEvent::Triggered, this, &ASTLocalPlayer::ChangeQuickSlot);
	EnhancedInputComponent->BindAction(ScrollQuickSlotAction, ETriggerEvent::Triggered, this, &ASTLocalPlayer::ScrollQuickSlot);
}

void ASTLocalPlayer::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set Local Player Mesh
	if(PlayerMeshes[static_cast<int>(PlayerMeshType)].IsValid())
	{
		USkeletalMesh* PlayerMesh = PlayerMeshes[static_cast<int>(PlayerMeshType)].LoadSynchronous();
		GetMesh()->SetSkeletalMesh(PlayerMesh);
	}

	HoldItem();
}

void ASTLocalPlayer::SetupHUDWidget(USTHUDWidget* InHUDWidget)
{
	ISTCharacterHUDInterface::SetupHUDWidget(InHUDWidget);

	if (InHUDWidget)
	{
		InHUDWidget->GetStatWidget()->SetStatComponent(StatComp);
		InHUDWidget->SetInventoryComponent(InventoryComp);
		InHUDWidget->SetQuickSlotComponent(QuickSlotComp);

		InHUDWidget->UpdateStat();
		InHUDWidget->UpdateQuickSlots();

		StatComp->OnStatChanged.AddUObject(InHUDWidget, &USTHUDWidget::UpdateStat);
		InventoryComp->OnInventoryUpdated.AddUObject(InHUDWidget, &USTHUDWidget::UpdateInventoryMenu);
		QuickSlotComp->OnQuickSlotUpdated.AddUObject(InHUDWidget, &USTHUDWidget::UpdateQuickSlots);

		// OnDrop 시 업데이트 연결
		for (int i = 0; i < QuickSlotComp->QuickSlots.Num(); i++)
		{
			InHUDWidget->GetQuickSlotWidget(i)->OnQuickSlotWidgetDrop.AddUObject(QuickSlotComp, &USTQuickSlotComponent::AddItem);
		}
	}
}

void ASTLocalPlayer::AttackHitCheck()
{
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * 100;

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	if (!bHit)
		return;

	AActor* HitActor = Hit.GetActor();
	if (!HitActor)
		return;

	ASTSliceableActor* SliceableActor = Cast<ASTSliceableActor>(HitActor);
	if (!SliceableActor)
	{
		return;
	}

	SliceableActor->Slice(Hit.ImpactPoint, Hit.ImpactNormal, this);

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		HitEffect,
		Hit.ImpactPoint,
		Hit.ImpactNormal.Rotation()
	);

	// 스태미나 감소
	StatComp->AddStamina(-1.f);

	OnStatUIUpdated();

#if ENABLE_DRAW_DEBUG
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.f);
#endif
}

void ASTLocalPlayer::SetCameraPose(ECameraPose NewPose)
{
	StartPose.SpringArmLength = SpringArmComp->TargetArmLength;
	StartPose.CameraY = CameraComp->GetRelativeLocation().Y;

	TargetPose = PoseSettings[NewPose];

	PoseElapsedTime = 0.f;
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

void ASTLocalPlayer::UseItem()
{
	if (!InventoryComp || !QuickSlotComp)
		return;

	int32 QuickSlotIndex = QuickSlotComp->CurrentSelectQuickSlotIndex;
	if (QuickSlotIndex == INDEX_NONE)
		return;

	int32 InventorySlotIndex = QuickSlotComp->InventorySlotIndex[QuickSlotIndex];
	if (InventorySlotIndex == INDEX_NONE)
		return;

	// 결과
	FSTItemSlot OutSlot;

	// 아이템 사용
	EItemUseType Result = InventoryComp->UseItem(
		InventorySlotIndex,
		this,
		0,
		OutSlot
	);

	// 사용 결과에 따른 분기 효과(Implement)
	UseItemEffect(OutSlot, Result);

	HoldItem();
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
	case EItemType::Letter:
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

void ASTLocalPlayer::ShoulderMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void ASTLocalPlayer::ShoulderLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ASTLocalPlayer::PistolAim(const FInputActionValue& Value)
{
	bool bIsAiming = HasAnyState(EPlayerState::Aiming);
	if (bIsAiming)
	{
		RemoveState(EPlayerState::Aiming);
		ApplyStateSettings(ECameraPose::Idle);
		bUseControllerRotationYaw = false;
	}
	else
	{
		AddState(EPlayerState::Aiming);
		ApplyStateSettings(ECameraPose::Aiming);
		bUseControllerRotationYaw = true;
	}
}

void ASTLocalPlayer::PistolFire(const FInputActionValue& Value)
{
	// TODO: 좌클릭 처리를 여기서 분기하기, 함수 이름 변경하기
	bool bIsAiming = HasAnyState(EPlayerState::Aiming);
	if (bIsAiming)
	{
		//TODO: 파티클 애니메이션 재생
	}
}

void ASTLocalPlayer::ChangeQuickSlot(const FInputActionValue& Value)
{
	int32 SlotIndex = FMath::RoundToInt(Value.Get<float>());

	// TODO: 퀵슬롯 변경 로직 작성
}

void ASTLocalPlayer::ScrollQuickSlot(const FInputActionValue& Value)
{
	// TODO: 스크롤로 퀵슬롯 번호 변경
	int32 ScrollValue = FMath::RoundToInt(Value.Get<float>());
	int32 CurrentIndex = QuickSlotComp->CurrentSelectQuickSlotIndex;

	if(ScrollValue > 0)
	{
		//CurrentIndex = (CurrentIndex + 1) % QuickSlotComp->QuickSlots.Num();
		UE_LOG(LogTemp, Log, TEXT("Scroll Up"));
	}
	else if(ScrollValue < 0)
	{
		//CurrentIndex = (CurrentIndex - 1 + QuickSlotComp->QuickSlots.Num()) % QuickSlotComp->QuickSlots.Num();
		UE_LOG(LogTemp, Log, TEXT("Scroll Down"));
	}
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
