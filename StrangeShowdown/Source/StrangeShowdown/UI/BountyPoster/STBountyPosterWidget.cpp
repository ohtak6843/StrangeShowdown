// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BountyPoster/STBountyPosterWidget.h"
#include "Components/WrapBox.h"
#include "UI/BountyPoster/STBountyPosterSlotWidget.h"

#include "Character/Player/STPlayerBase.h"

USTBountyPosterWidget::USTBountyPosterWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USTBountyPosterWidget::UpdateBountyPoster()
{
	// TODO: GameState에서 플레이어 가져와서 정렬 후 UI에 출력, 현재는 임시로 로컬 플레이어만 가져와서 출력
	USTBountyPosterSlotWidget* BountyPosterSlotWidget = CreateWidget<USTBountyPosterSlotWidget>(this, BountyPosterSlotWidgetClass);
	if(BountyPosterSlotWidget)
	{
		BountyPosterSlotWrapBox->AddChild(BountyPosterSlotWidget);
	}

	for (int i = 0; i < BountyPosterSlotWrapBox->GetChildrenCount(); i++)
	{
		if (USTBountyPosterSlotWidget* SlotWidget = Cast<USTBountyPosterSlotWidget>(BountyPosterSlotWrapBox->GetChildAt(i)))
		{
			SlotWidget->UpdateBountyPosterSlot(Cast<ASTPlayerBase>(GetOwningPlayerPawn()));
		}
	}
}
