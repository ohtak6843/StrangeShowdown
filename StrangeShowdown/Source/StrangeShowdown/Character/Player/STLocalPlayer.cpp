// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/STLocalPlayer.h"
#include "Character/Sheriff/STFieldSheriff.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Item/STPickupItem.h"
#include "Components/LineBatchComponent.h"
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
#include "UI/SheriffChaseTimer/STSheriffChaseTimerWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
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
	StatComp->Bounty = 0;
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

	// Sheriff Chase Check
	if (FieldSheriff)
	{
		SheriffChaseUpdate();
	}

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
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &ASTLocalPlayer::ShoulderMove);
	EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &ASTLocalPlayer::ShoulderLook);
	EnhancedInputComponent->BindAction(PistolAimAction, ETriggerEvent::Triggered, this, &ASTLocalPlayer::PistolAim);
	EnhancedInputComponent->BindAction(UseQuickSlotItemAction, ETriggerEvent::Triggered, this, &ASTLocalPlayer::UseQuickSlotItem);
	EnhancedInputComponent->BindAction(LookingUpAction, ETriggerEvent::Triggered, this, &ASTLocalPlayer::LookingUp);
	EnhancedInputComponent->BindAction(ChangeQuickSlotAction, ETriggerEvent::Triggered, this, &ASTLocalPlayer::ChangeQuickSlot);
	EnhancedInputComponent->BindAction(ScrollQuickSlotAction, ETriggerEvent::Triggered, this, &ASTLocalPlayer::ScrollQuickSlot);
	EnhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Triggered, this, &ASTLocalPlayer::PickUp);
	EnhancedInputComponent->BindAction(DropItemAction, ETriggerEvent::Triggered, this, &ASTLocalPlayer::DropItem);
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

	// Sheriff Chase Line
	LineBatcher = NewObject<ULineBatchComponent>(this, TEXT("LineBatcher"));
	LineBatcher->RegisterComponent();

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
		InHUDWidget->SetStoreComponent(StoreComp);

		InHUDWidget->SetWidgetType(EHUDWidgetType::Player);

		InHUDWidget->UpdateStat();
		InHUDWidget->UpdateQuickSlots();

		StatComp->OnStatChanged.AddUObject(InHUDWidget, &USTHUDWidget::UpdateStat);
		InventoryComp->OnInventoryUpdated.AddUObject(InHUDWidget, &USTHUDWidget::UpdateInventoryMenu);
		QuickSlotComp->OnQuickSlotUpdated.AddUObject(InHUDWidget, &USTHUDWidget::UpdateQuickSlots);
		StoreComp->OnStoreUpdated.AddUObject(InHUDWidget, &USTHUDWidget::UpdateStoreMenu);

		HUDWidget = InHUDWidget;
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

	switch (Result)
	{
	case EItemUseType::CanUse:
		ShowFloatingMessage(OutSlot.ItemData->ItemName);
		break;
	case EItemUseType::NotEnoughStaminaCost:
		ShowFloatingMessage(FText::FromString(TEXT("스태미나 부족!")));
		if(ErrorSound.IsValid())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound.LoadSynchronous());
		}
		break;
	case EItemUseType::NoEffect:
		break;
	case EItemUseType::Exception:
		ShowFloatingMessage(FText::FromString(TEXT("사용 불가!")));
		if(ErrorSound.IsValid())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound.LoadSynchronous());
		}
		break;
	case EItemUseType::UnValid:
		break;
	}

	if (OutSlot.ItemData)
	{
		const int32 Cost = OutSlot.ItemData->StaminaCost;
		StatComp->AddStamina(-Cost);

		// 아이템 제거
		InventoryComp->RemoveItem(InventorySlotIndex, 1);
	}

	InventoryComp->OnInventoryUpdated.Broadcast();

	// 아이템 사용 후 퀵슬롯 업데이트
	QuickSlotComp->OnQuickSlotUpdated.Broadcast();

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

void ASTLocalPlayer::HandleStoreSlotClicked(const FStoreSlot& InStoreSlot)
{
	if (InStoreSlot.ItemData && false == InStoreSlot.bIsSold)
	{
		if (StatComp->Gold >= InStoreSlot.ItemData->GoldCost)
		{
			// 메세지 출력
			ShowFloatingMessage(FText::FromString(TEXT("구매 성공!")));

			// 사운드 재생
			if (BuySound.IsValid())
			{
				UGameplayStatics::PlaySound2D(GetWorld(), BuySound.LoadSynchronous());
			}

			// 골드 차감 및 아이템 추가
			StatComp->AddGold(-InStoreSlot.ItemData->GoldCost);

			int32 AddedInventoryIndex = -1;
			FSTItemSlot ItemSlot(InStoreSlot.ItemData, true, 1);
			bool bAdded = InventoryComp->AddItem(ItemSlot, AddedInventoryIndex);
			if (bAdded && QuickSlotComp)
			{
				for (int32 i = 0; i < QuickSlotComp->QuickSlots.Num(); i++)
				{
					FSTItemSlot& QuickSlot = QuickSlotComp->QuickSlots[i];
					if (QuickSlot.ItemData == InStoreSlot.ItemData)
					{
						QuickSlot.Count += 1;
						QuickSlotComp->OnQuickSlotUpdated.Broadcast();
						break;
					}
				}
			}

			if (!bAdded || AddedInventoryIndex == -1)
				return;

			// 퀵슬롯에 추가된 아이템이 없으면 자동으로 퀵슬롯에 추가
			if (QuickSlotComp)
			{
				int32 TargetQuickSlotIndex = -2;
				bool result = QuickSlotComp->AddItem(InventoryComp, AddedInventoryIndex, TargetQuickSlotIndex);
			}

			// 아이템 장착
			HoldItem();

			StoreComp->BuyItem(InStoreSlot.SlotIndex);
		}
		else
		{
			ShowFloatingMessage(FText::FromString(TEXT("골드 부족!")));
			if (ErrorSound.IsValid())
			{
				UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound.LoadSynchronous());
			}
		}
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

		int32 SlotIndex = 0;
		QuickSlotComp->SetCurrentSelectIndex(SlotIndex);
		HoldItem();
	}
}

void ASTLocalPlayer::LookingUp(const FInputActionValue& Value)
{
	bool bIsLookingUp = HasAnyState(EPlayerState::LookingUp);
	if (bIsLookingUp)
	{
		RemoveState(EPlayerState::LookingUp);
		ApplyStateSettings(ECameraPose::Idle);
		bUseControllerRotationYaw = false;
	}
	else
	{
		AddState(EPlayerState::LookingUp);
		ApplyStateSettings(ECameraPose::LookingUp);
		bUseControllerRotationYaw = true;
	}
}

void ASTLocalPlayer::UseQuickSlotItem(const FInputActionValue& Value)
{
	USTItemDataAssetBase* CurrentItemData = QuickSlotComp->GetCurrentSelectedQuickSlotItemData();
	if (nullptr == CurrentItemData)
		return;

	switch (CurrentItemData->ItemType)
	{
		case EItemType::Pistol:
			if (HasAnyState(EPlayerState::Aiming) && AttackTraceComp->TracingFieldPlayer)
			{
				PistolFire();
			}
			break;
		case EItemType::Hammer:
			if (1.0f <= StatComp->CurrentStamina)
			{
				HammerSmash();
			}
			else
			{
				ShowFloatingMessage(FText::FromString(TEXT("스태미나 부족!")));
				if (ErrorSound.IsValid())
				{
					UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound.LoadSynchronous());
				}
			}
			break;
		case EItemType::Helmet:
		case EItemType::Meat:
		case EItemType::Whiskey:
		case EItemType::EnhancePower:
		case EItemType::Letter:
			UseItem();
			break;
	}
}

void ASTLocalPlayer::ChangeQuickSlot(const FInputActionValue& Value)
{
	int32 SlotIndex = FMath::RoundToInt(Value.Get<float>()) - 1;

	bool result = HasAnyState(EPlayerState::Aiming | EPlayerState::LookingUp);
	if(result)
	{
		ApplyStateSettings(ECameraPose::Idle);
	}
	QuickSlotComp->SetCurrentSelectIndex(SlotIndex);
	HoldItem();
}

void ASTLocalPlayer::ScrollQuickSlot(const FInputActionValue& Value)
{
	int32 ScrollValue = FMath::RoundToInt(Value.Get<float>());
	int32 CurrentIndex = QuickSlotComp->CurrentSelectQuickSlotIndex;

	if(ScrollValue > 0)
	{
		CurrentIndex = (CurrentIndex + 1) % QuickSlotComp->QuickSlots.Num();
	}
	else if(ScrollValue < 0)
	{
		CurrentIndex = (CurrentIndex - 1 + QuickSlotComp->QuickSlots.Num()) % QuickSlotComp->QuickSlots.Num();
	}

	QuickSlotComp->SetCurrentSelectIndex(CurrentIndex);
	HoldItem();
}

void ASTLocalPlayer::PickUp(const FInputActionValue& Value)
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

	FSTItemSlot ItemSlot(PickupItem->ItemData, true, 1);

	bool bAdded = InventoryComp->AddItem(
		ItemSlot,
		AddedInventoryIndex
	);

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

	PickupItem->Destroy();

	// 퀵슬롯에 추가된 아이템이 없으면 자동으로 퀵슬롯에 추가
	if (QuickSlotComp)
	{
		int32 TargetQuickSlotIndex = -2;
		bool result = QuickSlotComp->AddItem(InventoryComp, AddedInventoryIndex, TargetQuickSlotIndex);
	}

	// 아이템 장착
	HoldItem();
}

void ASTLocalPlayer::DropItem(const FInputActionValue& Value)
{
	USTItemDataAssetBase* ItemData = QuickSlotComp->GetCurrentSelectedQuickSlotItemData();
	if (nullptr == ItemData) return;

	if (ItemData->ItemType > EItemType::Hammer)
	{
		const FTransform SpawnTransform(GetActorRotation(), GetActorLocation() + GetActorForwardVector() * 100.0f);
		ASTPickupItem* PickupItem = GetWorld()->SpawnActorDeferred<ASTPickupItem>(ASTPickupItem::StaticClass(), SpawnTransform);
		if (PickupItem)
		{
			PickupItem->ItemData = ItemData;
			PickupItem->FinishSpawning(SpawnTransform);
		}
	}

	InventoryComp->RemoveItem(QuickSlotComp->InventorySlotIndex[QuickSlotComp->CurrentSelectQuickSlotIndex], 1);
	InventoryComp->OnInventoryUpdated.Broadcast();
	QuickSlotComp->OnQuickSlotUpdated.Broadcast();

	HoldItem();
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

void ASTLocalPlayer::SetFieldSheriff(ASTFieldSheriff* NewSheriff)
{
	FieldSheriff = NewSheriff;
	bIsChasingSheriff = true;

	HUDWidget->GetSheriffChaseTimerWidget()->SetTimer(10.f);

	// SheriffChaseTimerWidget visible
	if (HUDWidget.IsValid())
	{
		HUDWidget->GetSheriffChaseTimerWidget()->SetVisibility(ESlateVisibility::Visible);
	}
}

void ASTLocalPlayer::ClearSheriff()
{
	FieldSheriff = nullptr;
	bIsChasingSheriff = false;

	// SheriffChaseTimerWidget Hidden
	if (HUDWidget.IsValid())
	{
		HUDWidget->GetSheriffChaseTimerWidget()->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ASTLocalPlayer::SheriffChaseUpdate()
{
	if (!FieldSheriff) return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	// DPI ������ �� ����Ʈ ũ�� ���
	float DPIScale = UWidgetLayoutLibrary::GetViewportScale(GetWorld());

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	// �ȼ� �ػ󵵸� UMG ��ǥ�� ��ȯ
	FVector2D UMGViewport = ViewportSize / DPIScale;
	FVector2D ScreenCenter = UMGViewport * 0.5f;

	// ���Ȱ��� ���� ��ġ�� ȭ�� ��ǥ�� ��ȯ
	FVector SheriffLocation = FieldSheriff->GetActorLocation();
	FVector2D RawScreenPos;
	bool bProjected = PC->ProjectWorldLocationToScreen(SheriffLocation, RawScreenPos);

	// ���Ȱ��� ȭ�� ��ǥ�� DPI �����Ϸ� �����Ͽ� UMG ��ǥ�� ��ȯ
	FVector2D UMGPos = RawScreenPos / DPIScale;

	// ȭ�� �����ڸ����� ���Ȱ� �������� �ʹ� ������ �ٴ� ���� �����ϱ� ���� ���� ����
	const float EdgeMargin = 100.f;

	// ���Ȱ��� ȭ�� ���� �ִ��� Ȯ��
	bool bOnScreen = bProjected
		&& UMGPos.X > EdgeMargin
		&& UMGPos.X < UMGViewport.X - EdgeMargin
		&& UMGPos.Y > EdgeMargin
		&& UMGPos.Y < UMGViewport.Y - EdgeMargin;

	auto* TimerWidget = HUDWidget->GetSheriffChaseTimerWidget();

	// ���Ȱ��� ȭ�� ���� ������ ���Ȱ� �������� ���Ȱ� ��ġ�� ��ġ
	// ȭ�� �ۿ� ������ �����ڸ��� ��ġ
	if (bOnScreen)
	{
		TimerWidget->SetTimerWidgetLocation(UMGPos);
	}
	else
	{
		FVector2D Direction = UMGPos - ScreenCenter;
		Direction.Normalize();

		FVector2D HalfBound = ScreenCenter - FVector2D(EdgeMargin, EdgeMargin);

		float ScaleX = (Direction.X != 0.f) ? HalfBound.X / FMath::Abs(Direction.X) : FLT_MAX;
		float ScaleY = (Direction.Y != 0.f) ? HalfBound.Y / FMath::Abs(Direction.Y) : FLT_MAX;

		FVector2D EdgePos = ScreenCenter + Direction * FMath::Min(ScaleX, ScaleY);

		float ArrowAngle = FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X));

		TimerWidget->SetTimerWidgetLocation(EdgePos);
	}

	// LocalPlayer�� Head ���ϰ� ���Ȱ��� Head ������ ���� �ٷ� ����
	if (!FieldSheriff) return;

	FVector LocalPlayerHead = GetMesh()->GetSocketLocation(FName("Head"));
	FVector SheriffHead = FieldSheriff->GetMesh()->GetSocketLocation(FName("Head"));

	ClearPersistentLines();
	DrawPersistentLine(LocalPlayerHead, SheriffHead, FColor::Red, 2.f);
}

void ASTLocalPlayer::TestAddSheriffTransform()
{
	// �ӽÿ� FieldSheriff �߰�
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FVector SpawnLocation = GetActorLocation() + FVector(1000.f, 0.f, 0.f);
	FRotator SpawnRotation = FRotator::ZeroRotator;
	ASTFieldSheriff* SpawnedSheriff = GetWorld()->SpawnActor<ASTFieldSheriff>(ASTFieldSheriff::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
	SetFieldSheriff(SpawnedSheriff);
}

void ASTLocalPlayer::DrawPersistentLine(FVector Start, FVector End, FColor Color, float Thickness)
{
	if (!LineBatcher) return;

	LineBatcher->DrawLine(Start, End, Color, 0, Thickness, 0.f);
}

void ASTLocalPlayer::ClearPersistentLines()
{
	if (!LineBatcher) return;
	LineBatcher->Flush();
}

void ASTLocalPlayer::ActiveAlterFirstMapObject()
{
	UE_LOG(LogTemp, Log, TEXT("Activated Alter First Map Object"));
}

void ASTLocalPlayer::ActiveBankFirstMapObject()
{
	UE_LOG(LogTemp, Log, TEXT("Activated Bank First Map Object"));
}

void ASTLocalPlayer::ActiveChurchFirstMapObject()
{
	UE_LOG(LogTemp, Log, TEXT("Activated Church First Map Object"));
}

void ASTLocalPlayer::ActiveStationFirstMapObject()
{
	UE_LOG(LogTemp, Log, TEXT("Activated Station First Map Object"));
}

void ASTLocalPlayer::ActivePubFirstMapObject()
{
	UE_LOG(LogTemp, Log, TEXT("Activated Pub First Map Object"));
}

void ASTLocalPlayer::SendMovePacket(const float DeltaTime)
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
