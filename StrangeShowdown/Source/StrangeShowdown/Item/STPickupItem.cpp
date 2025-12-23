// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/STPickupItem.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"

// Sets default values
ASTPickupItem::ASTPickupItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Mesh Component
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	SubMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SubMesh"));
	SubMesh->SetupAttachment(Mesh);

	// Initialize Collision Component
	PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
	PickupCollision->SetupAttachment(RootComponent);
	PickupCollision->InitSphereRadius(50.f); // default

	// Widget Component
	PickupWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidgetComponent->SetupAttachment(RootComponent);
	PickupWidgetComponent->InitWidget();
	PickupWidgetComponent->SetVisibility(false);

	// 카메라는 플레이어를 바라보도록
	PickupWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
}

// 에디터 배치 시 호출
void ASTPickupItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// 에디터에서 값 변경 시에도 Mesh 위치 갱신
	if (Mesh && ItemData)
	{
		// Mesh 설정
		Mesh->SetStaticMesh(ItemData->PickupMesh);
		// DataAsset에 저장된 스케일 적용
		Mesh->SetWorldScale3D(ItemData->MeshScale);
		// 위치 조정
		Mesh->SetRelativeLocation(MeshPos);
		// sub Mesh가 있으면 설정
		if (ItemData->PickupSubMesh)
		{
			if (!SubMesh)
			{
				SubMesh = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("SubMesh"));
				SubMesh->RegisterComponent();
				SubMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			}
			SubMesh->SetStaticMesh(ItemData->PickupSubMesh);
			SubMesh->SetWorldScale3D(ItemData->MeshScale);
		}
		else
		{
			if (SubMesh)
			{
				SubMesh->DestroyComponent();
				SubMesh = nullptr;
			}
		}
	}
}

// Called when the game starts or when spawned
void ASTPickupItem::BeginPlay()
{
	Super::BeginPlay();

	PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &ASTPickupItem::HandleBeginOverlap);
	PickupCollision->OnComponentEndOverlap.AddDynamic(this, &ASTPickupItem::HandleEndOverlap);
}

void ASTPickupItem::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacter* Player = Cast<ACharacter>(OtherActor))
	{
		OverlappedPlayer = Player;
		OnPlayerEnter.Broadcast();

		// 위젯 표시
		if (PickupWidgetComponent)
			PickupWidgetComponent->SetVisibility(true);
	}
}

void ASTPickupItem::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 BodyIndex)
{
	if (ACharacter* Player = Cast<ACharacter>(OtherActor))
	{
		OverlappedPlayer = nullptr;
		OnPlayerExit.Broadcast();

		// UI 숨기기
		if (PickupWidgetComponent)
			PickupWidgetComponent->SetVisibility(false);
	}
}

// Called every frame
void ASTPickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 틱마다 회전
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += DeltaTime * 45.f; // 초당 45도 회전
	SetActorRotation(NewRotation);

	// 위젯이 카메라를 바라보도록 처리
	if (PickupWidgetComponent)
	{
		FVector CameraLocation;
		FRotator CameraRotation;

		// 플레이어 카메라 얻기
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC && PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->GetCameraViewPoint(CameraLocation, CameraRotation);

			// UI가 카메라를 바라보게
			FVector Direction = CameraLocation - PickupWidgetComponent->GetComponentLocation();
			FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
			PickupWidgetComponent->SetWorldRotation(LookAtRotation);
		}
	}
}