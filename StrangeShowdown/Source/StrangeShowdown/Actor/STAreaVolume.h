// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "GameData/STTypes.h"
#include "STAreaVolume.generated.h"

UCLASS()
class STRANGESHOWDOWN_API ASTAreaVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTAreaVolume();

	void BringHUD();

protected:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Box;

	UPROPERTY(EditAnywhere)
	EAreaType AreaType;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class USTHUDWidget> HUDWidget;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};