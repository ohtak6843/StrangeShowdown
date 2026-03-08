// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/STCarriage.h"
#include "Components/BoxComponent.h"

// Sets default values
ASTCarriage::ASTCarriage()
{
	PrimaryActorTick.bCanEverTick = false;

	// Box
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
	BoxComponent->SetBoxExtent(FVector(70.f, 40.f, 55.f));
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	// Static Mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);
	StaticMesh->SetRelativeLocationAndRotation(FVector(-10.f, 0.f, -55.f), FRotator(0.f, -90.f, 0.f));

	// Set Static Mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CarriageMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/StrangeShowdown/Prop/Carriage/StaticMeshes/Carriage.Carriage'"));
	if (CarriageMeshRef.Object)
	{
		StaticMesh->SetStaticMesh(CarriageMeshRef.Object);
	}
}

// Called every frame
void ASTCarriage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASTCarriage::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Called when the game starts or when spawned
void ASTCarriage::BeginPlay()
{
	Super::BeginPlay();

}

