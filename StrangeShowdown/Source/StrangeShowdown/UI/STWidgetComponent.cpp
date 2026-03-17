// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/STWidgetComponent.h"
#include "UI/STUserWidget.h"

void USTWidgetComponent::InitWidget()
{
	Super::InitWidget();
	
	USTUserWidget* STUserWidget = Cast<USTUserWidget>(GetWidget());
	if (STUserWidget)
	{
		STUserWidget->SetOwningActor(GetOwner());
	}
}
