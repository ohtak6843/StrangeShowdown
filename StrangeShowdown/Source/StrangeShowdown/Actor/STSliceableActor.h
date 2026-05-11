#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STSliceableActor.generated.h"

UCLASS()
class STRANGESHOWDOWN_API ASTSliceableActor : public AActor
{
	GENERATED_BODY()

public:
	ASTSliceableActor();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:
	// 슬라이스 되었는지
	bool bIsSliced;

	// 도끼에서 호출(충돌 지점, 노멀)
	UFUNCTION(BlueprintCallable, Category = "Slice")
	virtual void Slice(const FVector& HitLocation, const FVector& HitNormal, class ASTPlayerBase* Player);

	// BP 연출
	UFUNCTION(BlueprintImplementableEvent)
	void OnSlicedBlueprint();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slice")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slice")
	UStaticMeshComponent* OutLineMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UMaterialInterface* OutLineMaterial;
};