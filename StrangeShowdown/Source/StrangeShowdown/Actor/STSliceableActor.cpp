#include "STSliceableActor.h"
#include "Character/Player/STPlayerBase.h"

ASTSliceableActor::ASTSliceableActor()
{
	bReplicates = true;
	SetReplicateMovement(true);

	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	MeshComponent->SetIsReplicated(true);

	bIsSliced = false;
}

void ASTSliceableActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASTSliceableActor::Slice(const FVector& HitLocation, const FVector& HitNormal, ASTPlayerBase* Player)
{
	if (bIsSliced)
	{
		return;
	}

	bIsSliced = true;

	OnSlicedBlueprint();
}