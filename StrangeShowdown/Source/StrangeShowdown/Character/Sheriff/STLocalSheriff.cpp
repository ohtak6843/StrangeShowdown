// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Sheriff/STLocalSheriff.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Component/STQuickSlotComponent.h"
#include "Component/STAttackTraceComponent.h"
#include "Character/Player/STFieldPlayer.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Actor/STMiniMapActor.h"
#include "Actor/STBigMapActor.h"

ASTLocalSheriff::ASTLocalSheriff()
{
	// Spring Arm Component
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	// Camera Component
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	// QuickSlot Component
	QuickSlotComp = CreateDefaultSubobject<USTQuickSlotComponent>(TEXT("QuickSlotComp"));

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
			UE_LOG(LogTemp, Warning, TEXT("MiniMapActor found: %s"), *MiniMap->GetName());
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
			UE_LOG(LogTemp, Warning, TEXT("BigMapActor found: %s"), *BigMap->GetName());
			break;
		}
	}

	InputMappingContextAdd();
}

void ASTLocalSheriff::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &ASTLocalSheriff::Move);
	EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &ASTLocalSheriff::Look);
	EnhancedInputComponent->BindAction(PistolAimAction, ETriggerEvent::Triggered, this, &ASTLocalSheriff::PistolAim);
	EnhancedInputComponent->BindAction(PistolFireAction, ETriggerEvent::Triggered, this, &ASTLocalSheriff::PistolFire);
}

void ASTLocalSheriff::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void ASTLocalSheriff::SetCameraPose(ECameraPose NewPose)
{
	StartPose.SpringArmLength = SpringArmComp->TargetArmLength;
	StartPose.CameraY = CameraComp->GetRelativeLocation().Y;

	TargetPose = PoseSettings[NewPose];

	PoseElapsedTime = 0.f;
}

void ASTLocalSheriff::ApplyStateSettings(ECameraPose NewState)
{
	switch (NewState)
	{
	case ECameraPose::Idle:
		ChangeToIdle();
		break;
	case ECameraPose::Aiming:
		ChangeToAiming();
		break;
	default:
		break;
	}
}

void ASTLocalSheriff::Tick(float DeltaTime)
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
}

void ASTLocalSheriff::ChangeToIdle()
{
	RemoveState(ESheriffState::Aiming);
	bUseControllerRotationYaw = false;

	SetCameraPose(ECameraPose::Idle);
}

void ASTLocalSheriff::ChangeToAiming()
{
	AddState(ESheriffState::Aiming);
	bUseControllerRotationYaw = true;

	SetCameraPose(ECameraPose::Aiming);
}

void ASTLocalSheriff::InputMappingContextAdd()
{
	// Input Mapping Context
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/Game/StrangeShowdown/Input/IMC_LocalSheriffInput.IMC_LocalSheriffInput"));
	if (nullptr != InputMappingContextRef.Object)
	{
		DefaultMappingContext = InputMappingContextRef.Object;
	}

	// Input Actions
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/StrangeShowdown/Input/Actions/IA_ShoulderMove.IA_ShoulderMove'"));
	if (nullptr != InputActionShoulderMoveRef.Object)
	{
		ShoulderMoveAction = InputActionShoulderMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/StrangeShowdown/Input/Actions/IA_ShoulderLook.IA_ShoulderLook'"));
	if (nullptr != InputActionShoulderLookRef.Object)
	{
		ShoulderLookAction = InputActionShoulderLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionPistolAimRef(TEXT("/Script/EnhancedInput.InputAction'/Game/StrangeShowdown/Input/Actions/IA_PistolAim.IA_PistolAim'"));
	if (nullptr != InputActionPistolAimRef.Object)
	{
		PistolAimAction = InputActionPistolAimRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionPistolFireRef(TEXT("/Script/EnhancedInput.InputAction'/Game/StrangeShowdown/Input/Actions/IA_PistolFire.IA_PistolFire'"));
	if (nullptr != InputActionPistolFireRef.Object)
	{
		PistolFireAction = InputActionPistolFireRef.Object;
	}
}

void ASTLocalSheriff::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void ASTLocalSheriff::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ASTLocalSheriff::PistolAim(const FInputActionValue& Value)
{
	bool bIsAiming = HasAnyState(ESheriffState::Aiming);
	if (bIsAiming)
	{
		RemoveState(ESheriffState::Aiming);
		ApplyStateSettings(ECameraPose::Idle);
		bUseControllerRotationYaw = false;
	}
	else
	{
		AddState(ESheriffState::Aiming);
		ApplyStateSettings(ECameraPose::Aiming);
		bUseControllerRotationYaw = true;
	}
}

void ASTLocalSheriff::PistolFire(const FInputActionValue& Value)
{
	bool bIsAiming = HasAnyState(ESheriffState::Aiming);
	if (bIsAiming)
	{
		// TODO: 파티클 출력
		if (AttackTraceComp)
		{
			Fire();
		}
	}
}

void ASTLocalSheriff::Fire()
{
	if (AttackTraceComp)
	{
		UGameplayStatics::ApplyDamage(
			AttackTraceComp->TracingFieldPlayer,
			1.f,
			GetController(),
			this,
			UDamageType::StaticClass()
		);

		Fire_BP();
	}
}