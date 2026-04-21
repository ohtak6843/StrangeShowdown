// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Title/STCustomComboBox.h"
#include "STCustomComboBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/ScrollBox.h"
#include "UI/Title/STCustomComboBoxItem.h"

void USTCustomComboBox::ToggleComboBox()
{
	if (bIsOpen)
	{
		CloseComboBox();
	}
	else
	{
		OpenComboBox();
	}
}

void USTCustomComboBox::OpenComboBox()
{
	if (DropDownPanel == nullptr)
		return;

	bIsOpen = true;
	DropDownPanel->SetVisibility(ESlateVisibility::Visible);
}

void USTCustomComboBox::CloseComboBox()
{
	if (DropDownPanel == nullptr)
		return;

	bIsOpen = false;
	DropDownPanel->SetVisibility(ESlateVisibility::Collapsed);
}

void USTCustomComboBox::SelectIndex(int32 Index)
{
	if (!Options.IsValidIndex(Index))
		return;

	SelectedIndex = Index;

	if (SelectedText)
	{
		SelectedText->SetVisibility(ESlateVisibility::Visible);
		SelectedText->SetText(Options[Index].DisplayText);
	}

	CloseComboBox();
	OnComboBoxSelected();
	OnSelectionChanged.Broadcast(SelectedIndex);
}

void USTCustomComboBox::RefreshOptions()
{
	if (!ScrollBox || !ComboBoxItemClass)
		return;

	ScrollBox->ClearChildren();

	for (int32 i = 0; i < Options.Num(); ++i)
	{
		USTCustomComboBoxItem* Item =
			CreateWidget<USTCustomComboBoxItem>(this, ComboBoxItemClass);

		if (!Item)
			continue;

		Item->SetItemData(i, Options[i].DisplayText, this);
		ScrollBox->AddChild(Item);
	}
}