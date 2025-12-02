// Fill out your copyright notice in the Description page of Project Settings.


#include "STPickupItem.h"

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

	// Initialize Collision Component
	PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
	PickupCollision->SetupAttachment(RootComponent);
	PickupCollision->InitSphereRadius(50.f); // default
}

// Called when the game starts or when spawned
void ASTPickupItem::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASTPickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 틱마다 회전
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += DeltaTime * 45.f; // 초당 45도 회전
	SetActorRotation(NewRotation);

}

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
	}
}