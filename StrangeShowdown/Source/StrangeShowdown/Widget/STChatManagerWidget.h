// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Misc/DateTime.h"
#include "STChatManagerWidget.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FChatMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString SenderNickName;

	UPROPERTY(BlueprintReadWrite)
	FString Message;

	UPROPERTY(BlueprintReadWrite)
	FDateTime Timestamp;
};

UCLASS()
class STRANGESHOWDOWN_API USTChatManagerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// UI Bind
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UScrollBox* ChatScrollBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* ChatInputTextBox;

public:
	UPROPERTY(BlueprintReadWrite, Category = "Chat")
	TArray<FChatMessage> ChatLog;


public:
	virtual void NativeConstruct() override;

	// 메시지 추가(UI 갱신)
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void AddChatMessage(const FString& SenderNickName, const FString& Message, FDateTime Time);

	// Enter 눌렀을 때 입력 처리, 입력 텍스트를 FChatMessage로 변환하여 ChatLog에 추가
	UFUNCTION()
	void OnChatInputCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	// ChatLog를 기반으로 UI 갱신
	void RefreshChatUI();

	// ChatManager 포커스
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void SetChatInputFocus();
};