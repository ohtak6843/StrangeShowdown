// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NPC/STStorekeeper.h"
#include "Components/CapsuleComponent.h"
#include "Component/STStoreComponent.h"
#include "Item/STItemDataAssetBase.h"
#include "Character/Player/STLocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	// Set Skeletal Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/CharactersUE4Mannequin/SK_Chr_Sheriff_01.SK_Chr_Sheriff_01'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	// Set Single Animation
	static ConstructorHelpers::FObjectFinder<UAnimationAsset> AnimationRef(TEXT("/Script/Engine.BlendSpace'/Game/PolygonWestern/EpicContent/Mannequin/RetargetAnimations/Unarmed/BS_UnarmedIdleWalkRun.BS_UnarmedIdleWalkRun'"));
	if (AnimationRef.Object)
	{
		GetMesh()->AnimationData.AnimToPlay = AnimationRef.Object;
		GetMesh()->AnimationData.bSavedLooping = true;
		GetMesh()->AnimationData.bSavedPlaying = true;
	}
}

void ASTStorekeeper::Interact_Implementation(APawn* Interactor)
{
	// Storekeeper가 가지고 있는 아이템을
	// 플레이어의 상점 컴포넌트 배열에 추가
	if (ASTLocalPlayer* Player = Cast<ASTLocalPlayer>(Interactor))
	{
		if (!Player->GetStoreComp()) return;

		USTStoreComponent* StoreComp = Player->GetStoreComp();
		int32 Count = FMath::Min(StoreComp->SlotCount, StoreItemPool.Num());

		for (int32 i = 0; i < Count; ++i)
		{
			StoreComp->StoreItemPool[i] = StoreItemPool[i];
		}

		Player->CreateStoreWidget();
	}
}

void ASTStorekeeper::BeginPlay()
{
	Super::BeginPlay();

	InteractCollision->OnComponentBeginOverlap.AddDynamic(this, &ASTStorekeeper::HandleBeginOverlap);
	InteractCollision->OnComponentEndOverlap.AddDynamic(this, &ASTStorekeeper::HandleEndOverlap);
}

void ASTStorekeeper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 위젯이 카메라를 바라보도록 처리
	if (InteractWidgetComponent)
	{
		FVector CameraLocation;
		FRotator CameraRotation;

		// 플레이어 카메라 얻기
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC && PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->GetCameraViewPoint(CameraLocation, CameraRotation);

			// UI가 카메라를 바라보게
			FVector Direction = CameraLocation - InteractWidgetComponent->GetComponentLocation();
			FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
			InteractWidgetComponent->SetWorldRotation(LookAtRotation);
		}
	}
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