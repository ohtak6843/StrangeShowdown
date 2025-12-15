// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "STItemUseEffect.h"
#include "STItemDataAssetBase.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	HealPack,
	WeaponEnforce,
	WirelessSet,
	BodyArmor,
	Temp
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class STRANGESHOWDOWN_API USTItemDataAssetBase : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	USTItemDataAssetBase();
	
public:
	// 아이템 고유 ID (인벤토리/저장용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ItemID;

	// UI 표시용 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemName;

	// 아이템 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText Description;

	// 인벤토리/픽업 UI에 표시할 아이콘
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* Icon;

	// 아이템 종류
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemType ItemType;

	// 픽업용 메쉬
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|World")
	UStaticMesh* PickupMesh;

	// 메쉬 크기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|World")
	FVector MeshScale = FVector(1.f, 1.f, 1.f);

	// Max Stack Count
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 MaxStack = 1;

	// 아이템 사용 효과
	UPROPERTY(EditAnywhere, Category = "Item")
	TSubclassOf<USTItemUseEffect> UseEffectClass;
};