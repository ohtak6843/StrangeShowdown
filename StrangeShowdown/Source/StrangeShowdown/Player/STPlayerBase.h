// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Component/STStatComponent.h"
#include "STPlayerBase.generated.h"

UENUM(BlueprintType)
enum class EPlayerMeshType : uint8
{
	Badguy	UMETA(DisplayName = "Badguy"),
	Cowboy	UMETA(DisplayName = "Cowboy"),
	Cowgirl	UMETA(DisplayName = "Cowgirl"),
	Gunman	UMETA(DisplayName = "Gunman")
};

UCLASS()
class STRANGESHOWDOWN_API ASTPlayerBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASTPlayerBase();

public:
	// Stat Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	TObjectPtr<USTStatComponent> StatComp;

	// Player Mesh Type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	EPlayerMeshType PlayerMeshType;

public:
	// Character Name
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Info")
	FString PlayerNickName = TEXT("NoName");

	// Character State
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State", Meta = (AllowPrivateAccess = "true"))
	uint8 bIsArmedRifle : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State", Meta = (AllowPrivateAccess = "true"))
	uint8 bIsAiming : 1;
};
