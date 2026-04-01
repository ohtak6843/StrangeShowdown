// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
};

UCLASS()
class STRANGESHOWDOWN_API USTChatManagerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USTChatManagerWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	class UEditableTextBox* GetChatInputTextBox() const { return ChatInputTextBox; }

	// 메시지 추가(UI 갱신)
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void AddChatMessage(const FString& SenderNickName, const FString& Message);

	// Enter 눌렀을 때 입력 처리, 입력 텍스트를 FChatMessage로 변환하여 ChatLog에 추가
	UFUNCTION()
	void OnChatInputCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	// ChatManager 포커스
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void SetChatInputFocus();

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UScrollBox> ChatScrollBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> ChatInputTextBox;

	UPROPERTY(BlueprintReadWrite, Category = "Chat")
	TArray<FChatMessage> ChatLog;
};