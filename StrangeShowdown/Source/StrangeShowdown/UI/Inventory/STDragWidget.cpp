// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/STDragWidget.h"
#include "Components/Image.h"

USTDragWidget::USTDragWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void USTDragWidget::NativeConstruct()
{
	Super::NativeConstruct();

	DragItemImage = Cast<UImage>(GetWidgetFromName(TEXT("ImageDragItem")));
}