// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/STChatManager.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Player/STPlayerBase.h"
#include "Framework/Application/SlateApplication.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void USTChatManager::NativeConstruct()
{
	Super::NativeConstruct();

	if (ChatInputTextBox)
	{
		ChatInputTextBox->OnTextCommitted.AddDynamic(this, &USTChatManager::OnChatInputCommitted);
	}
}

void USTChatManager::AddChatMessage(const FString& SenderNickName, const FString& Message)
{
	FChatMessage NewMessage;
	NewMessage.SenderNickName = SenderNickName;
	NewMessage.Message = Message;
	NewMessage.Timestamp = FDateTime::Now();
	ChatLog.Add(NewMessage);
	RefreshChatUI();
}

void USTChatManager::OnChatInputCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		FString InputMessage = Text.ToString();

		if (!InputMessage.IsEmpty())
		{
			APlayerController* PC = GetOwningPlayer();
			if (!PC) return;

			APawn* Pawn = PC->GetPawn();
			if (!Pawn) return;

			ASTPlayerBase* Player = Cast<ASTPlayerBase>(Pawn);
			if (!Player) return;

			FString NickName = Player->PlayerNickName;

			AddChatMessage(NickName, InputMessage);

			ChatInputTextBox->SetText(FText::GetEmpty());
		}

		if (APlayerController* PC = GetOwningPlayer())
		{
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = false;

			UWidgetBlueprintLibrary::SetFocusToGameViewport();
			PC->FlushPressedKeys();
		}
	}
}

void USTChatManager::RefreshChatUI()
{
	if (!ChatScrollBox) return;

	// 기존 메시지 제거
	ChatScrollBox->ClearChildren();
	for (const FChatMessage& ChatMessage : ChatLog)
	{
		// 새로운 텍스트 블록 생성 및 추가
		UTextBlock* MessageTextBlock = NewObject<UTextBlock>(ChatScrollBox);
		if (MessageTextBlock)
		{
			FString FormattedMessage = FString::Printf(TEXT("[%s] %s: %s"),
				*ChatMessage.Timestamp.ToString(TEXT("%H:%M")),
				*ChatMessage.SenderNickName,
				*ChatMessage.Message);

			MessageTextBlock->SetText(FText::FromString(FormattedMessage));
			MessageTextBlock->SetAutoWrapText(true);

			ChatScrollBox->AddChild(MessageTextBlock);

			// 스크롤을 가장 아래로 이동
			ChatScrollBox->ScrollToEnd();
		}
	}
}

void USTChatManager::SetChatInputFocus()
{
	if (ChatInputTextBox)
	{
		ChatInputTextBox->SetKeyboardFocus();
	}
}