// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Map/STMiniMapItemIconWidget.h"
#include "Components/Image.h"

void USTMiniMapItemIconWidget::SetIconTexture(UTexture2D* Texture)
{
	if (IconImage)
	{
		IconImage->SetBrushFromTexture(Texture);
	}
}