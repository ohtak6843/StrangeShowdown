// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Sheriff/STFieldSheriff.h"
#include "Character/Player/STLocalPlayer.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

ASTFieldSheriff::ASTFieldSheriff()
{
	// Collision Component
	InteractCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractCollision"));
	InteractCollision->SetupAttachment(RootComponent);
	InteractCollision->InitSphereRadius(200.f);

	// Widget Component
	InteractWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	InteractWidgetComponent->SetupAttachment(RootComponent);
	InteractWidgetComponent->InitWidget();
	InteractWidgetComponent->SetVisibility(false);

	InteractWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
}

void ASTFieldSheriff::BeginPlay()
{
	Super::BeginPlay();

	// CameraManager Ä³½Ì
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		CachedCameraManager = PC->PlayerCameraManager;
	}

	InteractCollision->OnComponentBeginOverlap.AddDynamic(this, &ASTFieldSheriff::HandleBeginOverlap);
	InteractCollision->OnComponentEndOverlap.AddDynamic(this, &ASTFieldSheriff::HandleEndOverlap);
}

void ASTFieldSheriff::Interact_Implementation(APawn* Interactor)
{
	
}

void ASTFieldSheriff::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!InteractWidgetComponent || !CachedCameraManager) return;

	FVector CameraLocation;
	FRotator CameraRotation;
	CachedCameraManager->GetCameraViewPoint(CameraLocation, CameraRotation);

	FVector Direction = CameraLocation - InteractWidgetComponent->GetComponentLocation();
	FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction).Rotator();

	InteractWidgetComponent->SetWorldRotation(LookAtRotation);

	// ·Î±× InteractWidgetComponentÀÇ visible ·Î±×
	UE_LOG(LogTemp, Warning, TEXT("InteractWidgetComponent Visible: %s"), InteractWidgetComponent->IsVisible() ? TEXT("True") : TEXT("False"));
}

void ASTFieldSheriff::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASTCharacter* Player = Cast<ASTCharacter>(OtherActor))
	{
		// À§Á¬ Ç¥½Ã
		if (InteractWidgetComponent)
			InteractWidgetComponent->SetVisibility(true);
	}
}

void ASTFieldSheriff::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 BodyIndex)
{
	if (ASTCharacter* Player = Cast<ASTCharacter>(OtherActor))
	{
		// UI ¼û±â±â
		if (InteractWidgetComponent)
			InteractWidgetComponent->SetVisibility(false);
	}
}