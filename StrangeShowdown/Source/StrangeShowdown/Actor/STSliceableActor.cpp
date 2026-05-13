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

	OutLineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OutLineMesh"));
	OutLineMesh->SetupAttachment(MeshComponent);

	OutLineMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/StrangeShowdown/OutLine/M_OutLine1.M_OutLine1"));

	MeshComponent->SetIsReplicated(true);

	bIsSliced = false;
}

void ASTSliceableActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASTSliceableActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (OutLineMesh && MeshComponent)
	{
		OutLineMesh->SetStaticMesh(MeshComponent->GetStaticMesh());
	}

	if (OutLineMaterial)
	{
		OutLineMesh->SetMaterial(0, OutLineMaterial);
	}
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