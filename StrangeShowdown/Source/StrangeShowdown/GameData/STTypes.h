#pragma once


#include "CoreMinimal.h"
#include "CommonEnum.h"
#include "STTypes.generated.h"

UENUM(BlueprintType)
enum class ECameraPose : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	Aiming		UMETA(DisplayName = "Aiming"),
	LookingUp	UMETA(DisplayName = "LookingUp")
};
ENUM_CLASS_FLAGS(ECameraPose)

USTRUCT(BlueprintType)
struct FCameraPoseSetting
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpringArmLength = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraY = 0.f;
};


UENUM(BlueprintType)
enum class EPlayerMeshType : uint8
{
	Badguy			UMETA(DisplayName = "Badguy"),
	BuisinessMan	UMETA(DisplayName = "BuisinessMan"),
	Cowboy			UMETA(DisplayName = "Cowboy"),
	Cowgirl			UMETA(DisplayName = "Cowgirl"),
	Gunman			UMETA(DisplayName = "Gunman"),
	Woman			UMETA(DisplayName = "Woman"),
	WorkingGirl		UMETA(DisplayName = "WorkingGirl")
};


UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EPlayerState : uint8
{
	None			= Common::PlayerState::None			UMETA(DisplayName = "None"),
	Idle			= Common::PlayerState::Idle			UMETA(DisplayName = "Idle"),
	Jumping			= Common::PlayerState::Jumping		UMETA(DisplayName = "Jumping"),
	HoldItem		= Common::PlayerState::HoldItem		UMETA(DisplayName = "HoldItem"),
	Aiming			= Common::PlayerState::Aiming		UMETA(DisplayName = "Aiming"),
	LookingUp		= Common::PlayerState::LookingUp	UMETA(DisplayName = "LookingUp"),
	Dead			= Common::PlayerState::Dead			UMETA(DisplayName = "Dead"),
};
ENUM_CLASS_FLAGS(EPlayerState)


UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ESheriffState : uint8
{
	None		= 0				UMETA(DisplayName = "None"),
	Idle		= 1 << 0		UMETA(DisplayName = "Idle"),
	Jumping		= 1 << 1		UMETA(DisplayName = "Jumping"),
	Aiming		= 1 << 2		UMETA(DisplayName = "Aiming"),
	Dead		= 1 << 3		UMETA(DisplayName = "Dead"),
};
ENUM_CLASS_FLAGS(ESheriffState)

UENUM(BlueprintType)
enum class EItemUseType : uint8
{
	CanUse,
	NotEnoughStaminaCost,
	NoEffect,
	Exception,
	UnValid
};

UENUM(BlueprintType)
enum class EHUDWidgetType : uint8
{
	Player,
	Ghost,
	Sheriff
};

UENUM(BlueprintType)
enum class EAreaType : uint8
{
	Bar,
	Church,
	Bank,
	Station,
	Alter,
};