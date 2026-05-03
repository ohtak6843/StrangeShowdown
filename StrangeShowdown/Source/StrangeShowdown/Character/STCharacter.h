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
	// todo: cham
	// uint64 PlayerID = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MiniMap")
	TObjectPtr<class ASTMiniMapActor> MiniMapActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BigMap")
	TObjectPtr<class ASTBigMapActor> BigMapActor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:


};
