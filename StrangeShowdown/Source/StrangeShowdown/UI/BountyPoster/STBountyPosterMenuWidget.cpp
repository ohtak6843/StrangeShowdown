// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BountyPoster/STBountyPosterMenuWidget.h"
#include "UI/BountyPoster/STBountyPosterWidget.h"

USTBountyPosterMenuWidget::USTBountyPosterMenuWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USTBountyPosterMenuWidget::UpdateBountyPosterMenu()
{
	if (BountyPosterWidget)
	{
		BountyPosterWidget->UpdateBountyPoster();
	}
}
