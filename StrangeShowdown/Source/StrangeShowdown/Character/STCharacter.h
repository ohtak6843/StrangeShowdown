// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "STCharacter.generated.h"

UCLASS()
class STRANGESHOWDOWN_API ASTCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASTCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Character Name
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Info")
	FString PlayerNickName = TEXT("NoName");

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MiniMap")
	TObjectPtr<class ASTMiniMapActor> MiniMapActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BigMap")
	TObjectPtr<class ASTBigMapActor> BigMapActor;


	// --
	// method
	// --
public:
	void Move(const FVector& Location, const FRotator& Rotator);
	void TickMove(float DeltaTime);
	void SendMovePacket(const float DeltaTime, const uint8 PlayerStateFlag = 0u);

private:
	float SendMoveDeltaTime{};

protected:
	FVector TargetLocation{};
	FRotator TargetRotation{};
	const float MoveSpeed{ 12.f };
	const float RotationSpeed{ 12.f };

public:
	uint64 GetID() const { return ID; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	uint64 ID{ 0 };

};
