// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/MapObject/STMapObjectBase.h"
#include "Character/Player/STLocalPlayer.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASTMapObjectBase::ASTMapObjectBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Collision Component
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	RootComponent = SphereCollision;
	SphereCollision->InitSphereRadius(100.f);

	InteractCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractCollision"));
	InteractCollision->SetupAttachment(SphereCollision);
	InteractCollision->InitSphereRadius(200.f);

	// Widget Component
	InteractWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	InteractWidgetComponent->SetupAttachment(RootComponent);
	InteractWidgetComponent->InitWidget();
	InteractWidgetComponent->SetVisibility(false);
	InteractWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
}

void ASTMapObjectBase::Interact_Implementation(APawn* Interactor)
{
	ActivationMapObject(Interactor);
}

// Called when the game starts or when spawned
void ASTMapObjectBase::BeginPlay()
{
	Super::BeginPlay();

	// CameraManager 캐싱
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		CachedCameraManager = PC->PlayerCameraManager;
	}

	InteractCollision->OnComponentBeginOverlap.AddDynamic(this, &ASTMapObjectBase::HandleBeginOverlap);
	InteractCollision->OnComponentEndOverlap.AddDynamic(this, &ASTMapObjectBase::HandleEndOverlap);
	
}

// Called every frame
void ASTMapObjectBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!InteractWidgetComponent) return;
	if (!CachedCameraManager) return;

	FVector CameraLocation;
	FRotator CameraRotation;

	CachedCameraManager->GetCameraViewPoint(CameraLocation, CameraRotation);

	// 카메라를 향하도록 회전
	FVector Direction = CameraLocation - InteractWidgetComponent->GetComponentLocation();
	FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	InteractWidgetComponent->SetWorldRotation(LookAtRotation);
}

void ASTMapObjectBase::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASTCharacter* Player = Cast<ASTCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Log, TEXT("Player entered interaction range"));

		// 위젯 표시
		if (InteractWidgetComponent)
			InteractWidgetComponent->SetVisibility(true);
	}
}

void ASTMapObjectBase::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 BodyIndex)
{
	if (ASTCharacter* Player = Cast<ASTCharacter>(OtherActor))
	{
		// UI 숨기기
		if (InteractWidgetComponent)
			InteractWidgetComponent->SetVisibility(false);
	}
}