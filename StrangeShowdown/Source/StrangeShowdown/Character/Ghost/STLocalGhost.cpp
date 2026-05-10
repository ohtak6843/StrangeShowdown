// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ghost/STLocalGhost.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Actor/STMiniMapActor.h"
#include "Actor/STBigMapActor.h"
#include "Component/STStatComponent.h"
#include "Component/STQuickSlotComponent.h"
#include "UI/STHUDWidget.h"
#include "UI/Stat/STStatWidget.h"

#include "Controller/STSheriffController.h"
#include "Character/Sheriff/STLocalSheriff.h"
#include "Character/Sheriff/STFieldSheriff.h"
#include "GameFramework/GameModeBase.h"

ASTLocalGhost::ASTLocalGhost()
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
}

void ASTLocalGhost::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &ASTLocalGhost::ShoulderMove);
	EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &ASTLocalGhost::ShoulderLook);
	EnhancedInputComponent->BindAction(PossessSheriffAction, ETriggerEvent::Triggered, this, &ASTLocalGhost::PossessSheriff);
}

void ASTLocalGhost::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void ASTLocalGhost::SetupHUDWidget(USTHUDWidget* InHUDWidget)
{
	ISTCharacterHUDInterface::SetupHUDWidget(InHUDWidget);
	if (InHUDWidget)
	{
		InHUDWidget->GetStatWidget()->SetStatComponent(StatComp);
		InHUDWidget->SetQuickSlotComponent(QuickSlotComp);

		InHUDWidget->SetWidgetType(EHUDWidgetType::Ghost);

		InHUDWidget->UpdateStat();
		InHUDWidget->UpdateQuickSlots();

		StatComp->OnStatChanged.AddUObject(InHUDWidget, &USTHUDWidget::UpdateStat);
		QuickSlotComp->OnQuickSlotUpdated.AddUObject(InHUDWidget, &USTHUDWidget::UpdateQuickSlots);
	}
}

void ASTLocalGhost::ShoulderMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void ASTLocalGhost::ShoulderLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ASTLocalGhost::PossessSheriff(const FInputActionValue& Value)
{
	APlayerController* OldPC = Cast<APlayerController>(GetController());
	if (!OldPC)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller is not valid in PossessSheriff function"));
		return;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	OldPC->GetPlayerViewPoint(ViewLocation, ViewRotation);

	float TraceDistance = 500.f;
	FVector TraceEnd = ViewLocation + (ViewRotation.Vector() * TraceDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, ViewLocation, TraceEnd, ECC_Pawn, QueryParams);

#if ENABLE_DRAW_DEBUG
	FColor LineColor = bHit ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), ViewLocation, TraceEnd, LineColor, false, 2.0f, 0, 2.0f);
#endif

	// Possess Logic
	if (bHit)
	{
		ASTFieldSheriff* HitSheriff = Cast<ASTFieldSheriff>(HitResult.GetActor());
		if (HitSheriff)
		{
			FTransform SpawnTransform = HitSheriff->GetActorTransform();
			HitSheriff->Destroy();

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			ASTLocalSheriff* NewSheriff = GetWorld()->SpawnActor<ASTLocalSheriff>(SheriffClass, SpawnTransform, SpawnParams);
			if (NewSheriff)
			{
				ASTSheriffController* NewPC = GetWorld()->SpawnActor<ASTSheriffController>(SheriffControllerClass, ViewLocation, ViewRotation);
				if (NewPC)
				{
					OldPC->UnPossess();
					UGameplayStatics::GetGameMode(GetWorld())->SwapPlayerControllers(OldPC, NewPC);
					OldPC->Destroy();
					NewPC->Possess(NewSheriff);
					this->Destroy();
				}
			}
		}
	}
}
