// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/STPickupItem.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Actor/STMiniMapActor.h"
#include "Actor/STBigMapActor.h"
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

	// Collision Component
	PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
	PickupCollision->SetupAttachment(RootComponent);
	PickupCollision->InitSphereRadius(50.f);

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
		Mesh->SetStaticMesh(ItemData->ItemStaticMesh);
		// DataAsset에 저장된 스케일 적용 (Mesh의 상대 스케일)
		Mesh->SetRelativeScale3D(ItemData->MeshScale);
		// 위치 조정 (루트에 대한 상대 위치)
		Mesh->SetRelativeLocation(ItemData->MeshPos);
	}
}

// Called when the game starts or when spawned
void ASTPickupItem::BeginPlay()
{
	Super::BeginPlay();

	PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &ASTPickupItem::HandleBeginOverlap);
	PickupCollision->OnComponentEndOverlap.AddDynamic(this, &ASTPickupItem::HandleEndOverlap);

	// 미니맵 아이콘 설정
	APlayerController* PC = GetWorld()->GetFirstPlayerController();

	ASTCharacter* LocalPlayer = Cast<ASTCharacter>(PC->GetPawn());

	if (LocalPlayer && LocalPlayer->MiniMapActor)
	{
		LocalPlayer->MiniMapActor->RegisterItem(this);
	}
	if (LocalPlayer && LocalPlayer->BigMapActor)
	{
		LocalPlayer->BigMapActor->RegisterItem(this);
	}
}

void ASTPickupItem::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASTLocalPlayer* Player = Cast<ASTLocalPlayer>(OtherActor))
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
	if (ASTLocalPlayer* Player = Cast<ASTLocalPlayer>(OtherActor))
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