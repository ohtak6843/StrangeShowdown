// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NPC/STStorekeeper.h"
#include "Components/CapsuleComponent.h"
#include "Component/STStoreComponent.h"
#include "Item/STItemDataAssetBase.h"
#include "Character/Player/STLocalPlayer.h"
#include "Controller/STPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Prop/STCarriage.h"
#include "Kismet/GameplayStatics.h"
#include "Actor/STMiniMapActor.h"
#include "Actor/STBigMapActor.h"

ASTStorekeeper::ASTStorekeeper()
{
	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -104.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));

	// Collision Component
	InteractCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractCollision"));
	InteractCollision->SetupAttachment(RootComponent);
	InteractCollision->InitSphereRadius(200.f);

	// Widget Component
	InteractWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	InteractWidgetComponent->SetupAttachment(InteractCollision);
	InteractWidgetComponent->InitWidget();
	InteractWidgetComponent->SetVisibility(false);

	InteractWidgetComponent->SetWidgetSpace(EWidgetSpace::World);

	// Carriage
	Carriage = CreateDefaultSubobject<UChildActorComponent>(TEXT("Carriage"));
	Carriage->SetChildActorClass(ASTCarriage::StaticClass());
	Carriage->SetupAttachment(GetMesh());

	static ConstructorHelpers::FObjectFinder<UTexture2D> IconRef(
		TEXT("/Script/Engine.Texture2D'/Game/StrangeShowdown/UI/Texture/T_StoreIcon.T_StoreIcon'")
	);

	if (IconRef.Succeeded())
	{
		MiniMapIcon = IconRef.Object;
	}
}

void ASTStorekeeper::Interact_Implementation(APawn* Interactor)
{
	if (ASTLocalPlayer* Player = Cast<ASTLocalPlayer>(Interactor))
	{
		if (!Player->GetStoreComp()) return;

		USTStoreComponent* StoreComp = Player->GetStoreComp();

		StoreComp->CurrentStorekeeper = this;
		StoreComp->InitStore();

		// 플레이어(의 상점 컴포넌트) 등록
		if (!OpenedStores.Contains(StoreComp))
		{
			OpenedStores.Add(StoreComp);
		}

		if (ASTPlayerController* PC = Cast<ASTPlayerController>(Player->GetController()))
		{
			PC->OpenStore();
		}
	}
}

void ASTStorekeeper::BuyItem(int32 SlotIndex)
{
	if (!StoreItemPool.IsValidIndex(SlotIndex))
		return;

	if (StoreItemPool[SlotIndex] == nullptr)
		return;

	StoreItemPool[SlotIndex] = nullptr;

	UpdateOpenedStores();
}

void ASTStorekeeper::UnregisterStore(USTStoreComponent* StoreComp)
{
	OpenedStores.Remove(StoreComp);
}

void ASTStorekeeper::UpdateOpenedStores()
{
	for (USTStoreComponent* StoreComp : OpenedStores)
	{
		if (!StoreComp) continue;

		StoreComp->InitStore();
		StoreComp->RefreshStoreUI();
	}
}

void ASTStorekeeper::BeginPlay()
{
	Super::BeginPlay();

	// CameraManager 캐싱
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		CachedCameraManager = PC->PlayerCameraManager;

		ASTCharacter* LocalPlayer = Cast<ASTCharacter>(PC->GetPawn());

		if (LocalPlayer && LocalPlayer->MiniMapActor)
		{
			LocalPlayer->MiniMapActor->RegisterMiniMapTarget(this);
		}

		if (LocalPlayer && LocalPlayer->BigMapActor)
		{
			LocalPlayer->BigMapActor->RegisterBigMapTarget(this);
		}
	}

	InteractCollision->OnComponentBeginOverlap.AddDynamic(this, &ASTStorekeeper::HandleBeginOverlap);
	InteractCollision->OnComponentEndOverlap.AddDynamic(this, &ASTStorekeeper::HandleEndOverlap);
}

void ASTStorekeeper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!InteractWidgetComponent) return;
	if (!CachedCameraManager) return;

	FVector CameraLocation;
	FRotator CameraRotation;

	CachedCameraManager->GetCameraViewPoint(CameraLocation, CameraRotation);

	FVector Direction = CameraLocation - InteractWidgetComponent->GetComponentLocation();
	FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction).Rotator();

	InteractWidgetComponent->SetWorldRotation(LookAtRotation);
}

void ASTStorekeeper::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASTCharacter* Player = Cast<ASTCharacter>(OtherActor))
	{
		OverlappedPlayer = Player;
		OnPlayerEnter.Broadcast();

		// 위젯 표시
		if (InteractWidgetComponent)
			InteractWidgetComponent->SetVisibility(true);
	}
}

void ASTStorekeeper::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 BodyIndex)
{
	if (ASTCharacter* Player = Cast<ASTCharacter>(OtherActor))
	{
		OverlappedPlayer = nullptr;
		OnPlayerExit.Broadcast();

		// UI 숨기기
		if (InteractWidgetComponent)
			InteractWidgetComponent->SetVisibility(false);
	}
}

FVector ASTStorekeeper::GetMiniMapLocation_Implementation()
{
	return GetActorLocation();
}

UTexture2D* ASTStorekeeper::GetMiniMapIcon_Implementation()
{
	return MiniMapIcon;
}