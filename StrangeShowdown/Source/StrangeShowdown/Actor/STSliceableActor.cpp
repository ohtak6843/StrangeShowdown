#include "STSliceableActor.h"

ASTSliceableActor::ASTSliceableActor()
{
	bReplicates = true;
	SetReplicateMovement(true);

	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	MeshComponent->SetIsReplicated(true);

	bIsSliced = false;
}

void ASTSliceableActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASTSliceableActor::Slice(const FVector& HitLocation, const FVector& HitNormal)
{
	if (bIsSliced)
	{
		return;
	}

	bIsSliced = true;

	// 객체가 부숴졌을 떄 처리할 시스템 추가
	// 객체별로 보상이 다르므로 BP에서 처리할지 C++ 자식 클래스 만들어서 처리할지 고민 필요
	
	// BP 연출
	OnSliced();
}