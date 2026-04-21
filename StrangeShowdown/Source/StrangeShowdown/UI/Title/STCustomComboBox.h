// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STCustomComboBox.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FComboBoxOption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value;
};

UCLASS(Abstract)
class STRANGESHOWDOWN_API USTCustomComboBox : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 콤보 박스 토글 함수
	UFUNCTION(BlueprintCallable)
	void ToggleComboBox();

	// 콤보 박스 오픈 함수
	UFUNCTION(BlueprintCallable)
	void OpenComboBox();

	// 콤보 박스 클로즈 함수
	UFUNCTION(BlueprintCallable)
	void CloseComboBox();

	// 인덱스 선택 함수
	UFUNCTION(BlueprintCallable)
	void SelectIndex(int32 Index);

protected:
	// 콤보박스가 선택됐을 때 BP에서 처리
	UFUNCTION(BlueprintImplementableEvent, Category = "ComboBox")
	void OnComboBoxSelected();

public:
	// 콤보 박스 선택 변경 델리게이트
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
		FOnComboBoxSelectionChanged,
		int32, SelectedIndex
	);

public:
	// 선택 변경시 호출되는 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnComboBoxSelectionChanged OnSelectionChanged;

public:
	// 선택된 인덱스
	UPROPERTY(BlueprintReadOnly, Category = "ComboBox")
	int32 SelectedIndex = INDEX_NONE;

	// 콤보 박스가 오픈 되었는지
	UPROPERTY(BlueprintReadOnly, Category = "ComboBox")
	bool bIsOpen = false;

	// 옵션 담은 배열(실제 데이터)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ComboBox")
	TArray<FComboBoxOption> Options;

protected:
	// 콤보박스 아이템 BP 클래스(데이터 껍데기)
	UPROPERTY(EditDefaultsOnly, Category = "ComboBox")
	TSubclassOf<class USTCustomComboBoxItem> ComboBoxItemClass;

	// 옵션 새로고침 함수
	UFUNCTION(BlueprintCallable)
	void RefreshOptions();

protected:
	// 선택 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* SelectedButton;

	// 선택된 텍스트
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SelectedText;

	// 드롭다운 패널
	UPROPERTY(meta = (BindWidget))
	class UBorder* DropDownPanel;

	// 옵션 리스트
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ScrollBox;
};
