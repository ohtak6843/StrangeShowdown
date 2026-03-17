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

public:
	// 슬라이스 되었는지
	bool bIsSliced;

	// 도끼에서 호출(충돌 지점, 노멀)
	UFUNCTION(BlueprintCallable, Category = "Slice")
	virtual void Slice(const FVector& HitLocation, const FVector& HitNormal);

	// BP 연출/보상
	UFUNCTION(BlueprintImplementableEvent)
	void OnSlicedBlueprint();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slice")
	UStaticMeshComponent* MeshComponent;
};