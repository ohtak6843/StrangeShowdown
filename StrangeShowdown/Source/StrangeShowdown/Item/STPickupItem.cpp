#include "Item/STPickupItem.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Actor/STMiniMapActor.h"
#include "Actor/STBigMapActor.h"

// Sets default values
ASTPickupItem::ASTPickupItem()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	SubMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SubMesh"));
	SubMesh->SetupAttachment(Mesh);

	PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
	PickupCollision->SetupAttachment(RootComponent);
	PickupCollision->InitSphereRadius(50.f);

	PickupWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidgetComponent->SetupAttachment(RootComponent);
	PickupWidgetComponent->InitWidget();
	PickupWidgetComponent->SetVisibility(false);

	PickupWidgetComponent->SetWidgetSpace(EWidgetSpace::World);

	static ConstructorHelpers::FObjectFinder<UTexture2D> IconRef(
		TEXT("/Script/Engine.Texture2D'/Game/StrangeShowdown/UI/Texture/T_ItemIcon.T_ItemIcon'")
	);

	if (IconRef.Succeeded())
	{
		MiniMapIcon = IconRef.Object;
	}
}

void ASTPickupItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (Mesh && ItemData)
	{
		Mesh->SetStaticMesh(ItemData->ItemStaticMesh);
		Mesh->SetRelativeScale3D(ItemData->MeshScale);
		Mesh->SetRelativeLocation(ItemData->MeshPos);
	}
}

void ASTPickupItem::BeginPlay()
{
	Super::BeginPlay();

	PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &ASTPickupItem::HandleBeginOverlap);
	PickupCollision->OnComponentEndOverlap.AddDynamic(this, &ASTPickupItem::HandleEndOverlap);

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
}

void ASTPickupItem::HandleBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 BodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (ASTLocalPlayer* Player = Cast<ASTLocalPlayer>(OtherActor))
	{
		OverlappedPlayer = Player;
		OnPlayerEnter.Broadcast();

		if (PickupWidgetComponent)
			PickupWidgetComponent->SetVisibility(true);
	}
}

void ASTPickupItem::HandleEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 BodyIndex)
{
	if (ASTLocalPlayer* Player = Cast<ASTLocalPlayer>(OtherActor))
	{
		OverlappedPlayer = nullptr;
		OnPlayerExit.Broadcast();

		if (PickupWidgetComponent)
			PickupWidgetComponent->SetVisibility(false);
	}
}

FVector ASTPickupItem::GetMiniMapLocation_Implementation()
{
	return GetActorLocation();
}

UTexture2D* ASTPickupItem::GetMiniMapIcon_Implementation()
{
	return MiniMapIcon;
}

void ASTPickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 아이템 회전
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += DeltaTime * 45.f;
	SetActorRotation(NewRotation);

	// 위젯이 카메라를 바라보도록 처리
	if (PickupWidgetComponent && CachedCameraManager)
	{
		FVector CameraLocation;
		FRotator CameraRotation;

		CachedCameraManager->GetCameraViewPoint(CameraLocation, CameraRotation);

		FVector Direction = CameraLocation - PickupWidgetComponent->GetComponentLocation();
		FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction).Rotator();

		PickupWidgetComponent->SetWorldRotation(LookAtRotation);
	}
}