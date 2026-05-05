// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/MapObject/STMapObjectBase.h"
#include "Character/Player/STLocalPlayer.h"

// Sets default values
ASTMapObjectBase::ASTMapObjectBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Collision Component
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->InitSphereRadius(100.f);

	InteractCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractCollision"));
	InteractCollision->SetupAttachment(RootComponent);
	InteractCollision->InitSphereRadius(200.f);

	// Widget Component
	InteractWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	InteractWidgetComponent->SetupAttachment(InteractCollision);
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

	InteractCollision->OnComponentBeginOverlap.AddDynamic(this, &ASTMapObjectBase::HandleBeginOverlap);
	InteractCollision->OnComponentEndOverlap.AddDynamic(this, &ASTMapObjectBase::HandleEndOverlap);
	
}

// Called every frame
void ASTMapObjectBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASTMapObjectBase::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASTCharacter* Player = Cast<ASTCharacter>(OtherActor))
	{
		OnPlayerEnter.Broadcast();

		// À§Á¬ Ç¥½Ã
		if (InteractWidgetComponent)
			InteractWidgetComponent->SetVisibility(true);
	}
}

void ASTMapObjectBase::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 BodyIndex)
{
	if (ASTCharacter* Player = Cast<ASTCharacter>(OtherActor))
	{
		OnPlayerExit.Broadcast();

		// UI ¼û±â±â
		if (InteractWidgetComponent)
			InteractWidgetComponent->SetVisibility(false);
	}
}