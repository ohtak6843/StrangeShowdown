// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Stat/STStatWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/WrapBox.h"
#include "Component/STStatComponent.h"

USTStatWidget::USTStatWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USTStatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	int32 StaminaCount = 5;
	for (int32 i = 0; i < StaminaCount; i++)
	{
		UImage* StaminaImage = Cast<UImage>(GetWidgetFromName(FName(*FString::Printf(TEXT("StaminaImage%d"), i))));
		ensure(StaminaImage);

		StaminaImages.Add(StaminaImage);
	}

	int32 ActionCount = 6;
	for (int32 i = 0; i < ActionCount; i++)
	{
		UImage* ActionImage = Cast<UImage>(GetWidgetFromName(FName(*FString::Printf(TEXT("ActionImage%d"), i))));
		ensure(ActionImage);

		ActionImages.Add(ActionImage);
	}
}

void USTStatWidget::SetStatComponent(USTStatComponent* InStatComponent)
{
	if (InStatComponent)
	{
		SourceStatComp = InStatComponent;
	}
}

void USTStatWidget::SetWidgetType(EHUDWidgetType InWidgetType)
{
	StopAllAnimations();

	switch (InWidgetType)
	{
		case EHUDWidgetType::Player:
			SetPlayerWidget();
			break;
		case EHUDWidgetType::Ghost:
			SetGhostWidget();
			break;
		case EHUDWidgetType::Sheriff:
			SetSheriffWidget();
			break;
	}
}

void USTStatWidget::SetPlayerWidget()
{
	HeartImage->SetBrushFromTexture(PlayerHeartTexture);

	HpBar->SetVisibility(ESlateVisibility::Visible);
	HpText->SetVisibility(ESlateVisibility::Visible);
	ShieldBar->SetVisibility(ESlateVisibility::Visible);
	ShieldText->SetVisibility(ESlateVisibility::Visible);
	GoldImage->SetVisibility(ESlateVisibility::Visible);
	GoldText->SetVisibility(ESlateVisibility::Visible);
	StaminaWrapBox->SetVisibility(ESlateVisibility::Visible);
	ActionWrapBox->SetVisibility(ESlateVisibility::Visible);

	GhostGaugeBar->SetVisibility(ESlateVisibility::Hidden);
	SheriffGaugeBar->SetVisibility(ESlateVisibility::Hidden);

	PlayAnimation(PlayerHeartbeat, 0.f, 0);
}

void USTStatWidget::SetGhostWidget()
{
	HeartImage->SetBrushFromTexture(GhostHeartTexture);

	GhostGaugeBar->SetVisibility(ESlateVisibility::Visible);

	HpBar->SetVisibility(ESlateVisibility::Hidden);
	HpText->SetVisibility(ESlateVisibility::Hidden);
	ShieldBar->SetVisibility(ESlateVisibility::Hidden);
	ShieldText->SetVisibility(ESlateVisibility::Hidden);
	GoldImage->SetVisibility(ESlateVisibility::Hidden);
	GoldText->SetVisibility(ESlateVisibility::Hidden);
	StaminaWrapBox->SetVisibility(ESlateVisibility::Hidden);
	ActionWrapBox->SetVisibility(ESlateVisibility::Hidden);
	SheriffGaugeBar->SetVisibility(ESlateVisibility::Hidden);

	PlayAnimation(GhostHeartbeat, 0.f, 0);

	// TODO: 유령 게이지 감소하는 타이머 추가
}

void USTStatWidget::SetSheriffWidget()
{
	HeartImage->SetBrushFromTexture(SheriffHeartTexture);

	SheriffGaugeBar->SetVisibility(ESlateVisibility::Visible);

	HpBar->SetVisibility(ESlateVisibility::Hidden);
	HpText->SetVisibility(ESlateVisibility::Hidden);
	ShieldBar->SetVisibility(ESlateVisibility::Hidden);
	ShieldText->SetVisibility(ESlateVisibility::Hidden);
	GoldImage->SetVisibility(ESlateVisibility::Hidden);
	GoldText->SetVisibility(ESlateVisibility::Hidden);
	StaminaWrapBox->SetVisibility(ESlateVisibility::Hidden);
	ActionWrapBox->SetVisibility(ESlateVisibility::Hidden);

	PlayAnimation(SheriffHeartbeat, 0.f, 0);

	// TODO: 보안관 게이지 감소하는 타이머 추가
}

void USTStatWidget::UpdateStat()
{
	if (SourceStatComp.IsValid())
	{
		USTStatComponent* StatComp = SourceStatComp.Get();

#pragma region HP Update
		float CurrentHp = StatComp->CurrentHp;
		float MaxHp = StatComp->MaxHp;

		if (HpBar)
		{
			HpBar->SetPercent(CurrentHp / MaxHp);
		}

		if (HpText)
		{
			// TODO: 나중에 스탯 관련 정보 int에서 float으로 변경되면 변경해야 할 부분
			FString HpString = FString::Printf(TEXT("%.0f / %.0f"), CurrentHp, MaxHp);
			HpText->SetText(FText::FromString(HpString));
		}
#pragma endregion

#pragma region Shield Update
		float CurrentArmor = StatComp->CurrentArmor;
		float MaxArmor = StatComp->MaxArmor;

		if (ShieldBar)
		{
			ShieldBar->SetPercent(CurrentArmor / MaxArmor);
		}

		if (ShieldText)
		{
			FString ShieldString = FString::Printf(TEXT("+ %.0f"), CurrentArmor);
			ShieldText->SetText(FText::FromString(ShieldString));
		}
#pragma endregion

#pragma region Gold Update
		int32 CurrentGold = StatComp->Gold;
		
		if (GoldText)
		{
			FString GoldString = FString::Printf(TEXT("%d"), CurrentGold);
			GoldText->SetText(FText::FromString(GoldString));
		}
#pragma endregion

#pragma region Stamina Update
		int32 CurrentStamina = StatComp->CurrentStamina;
		int32 MaxStamina = StatComp->MaxStamina;
		check(CurrentStamina >= 0);

		FLinearColor Cyan(0.0f, 0.5f, 1.0f, 1.f);
		for (int32 i = 0; i < CurrentStamina; i++)
		{
			if (StaminaImages[i])
			{
				StaminaImages[i]->SetBrushTintColor(Cyan);
			}
		}

		for (int32 i = CurrentStamina; i < MaxStamina; i++)
		{
			if (StaminaImages[i])
			{
				StaminaImages[i]->SetBrushTintColor(FLinearColor::White);
			}
		}
#pragma endregion

#pragma region Action Update
		int32 CurrentAction = StatComp->CurrentAction;
		int32 UsableAction = StatComp->UseAbleAction;
		int32 MaxAction = StatComp->MaxAction;
		check(CurrentAction >= 0);

		FLinearColor DarkGray(0.2f, 0.2f, 0.2f, 1.f);
		FLinearColor Transparent(0.f, 0.f, 0.f, 0.f);
		for (int32 i = 0; i < CurrentAction; i++)
		{
			if (ActionImages[i])
			{
				ActionImages[i]->SetBrushTintColor(FLinearColor::White);
			}
		}

		for (int32 i = CurrentAction; i < UsableAction; i++)
		{
			if (ActionImages[i])
			{
				ActionImages[i]->SetBrushTintColor(DarkGray);
			}
		}

		for (int32 i = UsableAction; i < MaxAction; i++)
		{
			if (ActionImages[i])
			{
				ActionImages[i]->SetBrushTintColor(Transparent);
			}
		}
#pragma endregion
	}
}
