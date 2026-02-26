// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/STChatManagerWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Character/Player/STPlayerBase.h"
#include "Framework/Application/SlateApplication.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void USTChatManagerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ChatInputTextBox)
	{
		ChatInputTextBox->OnTextCommitted.AddDynamic(this, &USTChatManagerWidget::OnChatInputCommitted);
	}
}

void USTChatManagerWidget::AddChatMessage(const FString& SenderNickName, const FString& Message, FDateTime Time)
{
	FChatMessage NewMessage;
	NewMessage.SenderNickName = SenderNickName;
	NewMessage.Message = Message;
	NewMessage.Timestamp = Time;
	ChatLog.Add(NewMessage);
	RefreshChatUI();
}

void USTChatManagerWidget::OnChatInputCommitted(const FText& Text, ETextCommit::Type CommitMethod)
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

			// 임시로 FDateTime::Now() 사용
			AddChatMessage(NickName, InputMessage, FDateTime::Now());

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

void USTChatManagerWidget::RefreshChatUI()
{
	if (!ChatScrollBox) return;

	ChatScrollBox->ClearChildren();

	for (const FChatMessage& ChatMessage : ChatLog)
	{
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
		}
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			if (ChatScrollBox)
			{
				ChatScrollBox->ScrollToEnd();
			}
		}, 0.01f, false);
}

void USTChatManagerWidget::SetChatInputFocus()
{
	if (ChatInputTextBox)
	{
		ChatInputTextBox->SetKeyboardFocus();
	}
}