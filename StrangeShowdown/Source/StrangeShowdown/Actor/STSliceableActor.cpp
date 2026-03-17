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

	OnSlicedBlueprint();
}