// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Title/STCustomComboBoxItem.h"
#include "UI/Title/STCustomComboBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void USTCustomComboBoxItem::NativeConstruct()
{
	Super::NativeConstruct();

	if (ItemButton)
	{
		ItemButton->OnClicked.AddDynamic(this, &USTCustomComboBoxItem::OnItemClicked);
	}
}

void USTCustomComboBoxItem::SetItemData(
	int32 InIndex,
	const FText& InText,
	USTCustomComboBox* InOwner)
{
	Index = InIndex;
	OwnerComboBox = InOwner;

	if (ItemText)
	{
		ItemText->SetText(InText);
	}
}

void USTCustomComboBoxItem::OnItemClicked()
{
	if (OwnerComboBox)
	{
		OwnerComboBox->SelectIndex(Index);
	}
}