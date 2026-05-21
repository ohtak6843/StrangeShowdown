// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Sheriff/STSheriffBase.h"
#include "Actor/STInteractableActor.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "STFieldSheriff.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGhostEnter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGhostExit);

UCLASS()
class STRANGESHOWDOWN_API ASTFieldSheriff : public ASTSheriffBase, public IInteractable
{
	GENERATED_BODY()

public:
	ASTFieldSheriff();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	virtual void Interact_Implementation(APawn* Interactor) override;

	// Interact UI 위젯
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI", Meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* InteractWidgetComponent;

	// 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interact")
	USphereComponent* InteractCollision;

	// Overlap 이벤트
	UPROPERTY(BlueprintAssignable)
	FOnGhostEnter OnGhostEnter;

	UPROPERTY(BlueprintAssignable)
	FOnGhostExit OnGhostExit;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsPossessed : 1;

private:
	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
	APlayerCameraManager* CachedCameraManager;
};
