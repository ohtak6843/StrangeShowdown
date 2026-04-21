// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BountyPoster/STBountyPosterSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Character/Player/STPlayerBase.h"
#include "Component/STStatComponent.h"

USTBountyPosterSlotWidget::USTBountyPosterSlotWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USTBountyPosterSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	int32 StaminaCount = 5;
	for (int32 i = 0; i < StaminaCount; i++)
	{
		UImage* StaminaImage = Cast<UImage>(GetWidgetFromName(FName(*FString::Printf(TEXT("StaminaImage%d"), i))));
		ensure(StaminaImage);
		StaminaImages.Add(StaminaImage);
	}
}

void USTBountyPosterSlotWidget::UpdateBountyPosterSlot(ASTPlayerBase* InPlayer)
{
	if (InPlayer)
	{
		PlayerBountyText->SetText(FText::AsNumber(InPlayer->StatComp->Bounty));
		PlayerNameText->SetText(FText::FromString(InPlayer->PlayerNickName));

		// TODO: 플레이어 2D 이미지로 설정해주기
		if(TempTexture)
		{
			PlayerImage->SetBrushFromTexture(TempTexture);
		}

		PlayerGold->SetText(FText::AsNumber(InPlayer->StatComp->Gold));

		PlayerHpBar->SetPercent(InPlayer->StatComp->CurrentHp / InPlayer->StatComp->MaxHp);
		FString HpString = FString::Printf(TEXT("%.0f / %.0f"), InPlayer->StatComp->CurrentHp, InPlayer->StatComp->MaxHp);
		PlayerHpText->SetText(FText::FromString(HpString));

		for (int i = 0; i < StaminaImages.Num(); i++)
		{
			if (i < FMath::FloorToInt(InPlayer->StatComp->CurrentStamina))
			{
				StaminaImages[i]->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				StaminaImages[i]->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}
