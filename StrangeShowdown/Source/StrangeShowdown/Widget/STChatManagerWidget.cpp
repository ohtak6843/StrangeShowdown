// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/STChatManagerWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Misc/DateTime.h"
#include "Character/Player/STPlayerBase.h"
#include "Character/STCharacter.h"
#include "Framework/Application/SlateApplication.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

USTChatManagerWidget::USTChatManagerWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USTChatManagerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ChatInputTextBox)
	{
		ChatInputTextBox->OnTextCommitted.AddDynamic(this, &USTChatManagerWidget::OnChatInputCommitted);
	}
}

void USTChatManagerWidget::AddChatMessage(const FString& SenderNickName, const FString& Message)
{
	FChatMessage NewMessage;
	NewMessage.SenderNickName = SenderNickName;
	NewMessage.Message = Message;

	ChatLog.Add(NewMessage);

	if (!ChatScrollBox) return;

	UTextBlock* MessageTextBlock = NewObject<UTextBlock>(this);
	if (MessageTextBlock)
	{
		FString FormattedMessage = FString::Printf(TEXT("%s: %s"),
			*SenderNickName,
			*Message);

		MessageTextBlock->SetText(FText::FromString(FormattedMessage));

		// 폰트 크기 조절
		MessageTextBlock->SetFont(FSlateFontInfo(FPaths::ProjectContentDir() / TEXT("Fonts/F_StrangeShowdown.ttf"), 20));
		
		MessageTextBlock->SetAutoWrapText(true);
		MessageTextBlock->SetWrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping);
		ChatScrollBox->AddChild(MessageTextBlock);

		// 레이아웃 강제 계산
		ChatScrollBox->ForceLayoutPrepass();

		// 정확한 폭 가져오기
		float ScrollWidth = ChatScrollBox->GetCachedGeometry().GetLocalSize().X;
		MessageTextBlock->SetWrapTextAt(ScrollWidth - 20.f);
		
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
			{
				if (ChatScrollBox)
				{
					ChatScrollBox->ScrollToEnd();
				}
			}, 0.01f, false);
	}
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

			ASTCharacter* Player = Cast<ASTCharacter>(Pawn);
			if (!Player) return;

			FString NickName = Player->PlayerNickName;

			// 임시로 FDateTime::Now() 사용
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

void USTChatManagerWidget::SetChatInputFocus()
{
	if (ChatInputTextBox)
	{
		ChatInputTextBox->SetKeyboardFocus();
	}
}