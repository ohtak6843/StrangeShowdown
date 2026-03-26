// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "STLobbyStatusWidget.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTLobbyStatusWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// 새로운 플레이어가 들어오면 ID를 받아 슬롯 추가
	void AddPlayerSlot(uint64 PlayerID);

	// ID를 받아 플레이어 Ready 상태를 업데이트
	void SetPlayerReady(uint64 PlayerID, bool bReady);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class USTLobbyStatusSlotWidget> SlotWidgetClass;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWrapBox* WrapBox;

private:
	// 이후 레디 참조를 위해 PlayerState를 키로, SlotWidget을 값으로 하는 맵
	TMap<uint64, USTLobbyStatusSlotWidget*> SlotMap;
};
