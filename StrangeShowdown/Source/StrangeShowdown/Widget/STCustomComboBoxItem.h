// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STCustomComboBoxItem.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTCustomComboBoxItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "ComboBox")
	int32 Index = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "ComboBox")
	class USTCustomComboBox* OwnerComboBox;

	// 데이터 주입 함수
	void SetItemData(int32 InIndex, const FText& InText, USTCustomComboBox* InOwner);

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* ItemButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemText;

	// UUserWidget interface
	virtual void NativeConstruct() override;

	// 아이템 클릭 이벤트 핸들러
	UFUNCTION()
	void OnItemClicked();
};
