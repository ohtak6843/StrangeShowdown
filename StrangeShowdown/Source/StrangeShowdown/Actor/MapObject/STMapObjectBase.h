// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actor/STInteractableActor.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "STMapObjectBase.generated.h"

/**
 *
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterMapObjectEnter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterMapObjectExit);

UCLASS()
class STRANGESHOWDOWN_API ASTMapObjectBase : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTMapObjectBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(APawn* Interactor) override;

	virtual void ActivationMapObject(APawn* Interactor) {};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI", Meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* InteractWidgetComponent;

	// 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
	USphereComponent* SphereCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interact")
	USphereComponent* InteractCollision;

	// Overlap 이벤트
	UPROPERTY(BlueprintAssignable)
	FOnCharacterMapObjectEnter OnPlayerEnter;

	UPROPERTY(BlueprintAssignable)
	FOnCharacterMapObjectExit OnPlayerExit;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
