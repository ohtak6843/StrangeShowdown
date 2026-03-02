// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/STMiniMapWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Item/STPickupItem.h"

void USTMiniMapWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (!PlayerTexture) return;

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	APawn* Pawn = PC->GetPawn();
	if (!Pawn) return;

	USkeletalMeshComponent* Mesh =
		Pawn->FindComponentByClass<USkeletalMeshComponent>();
	if (!Mesh) return;

	float MeshYaw = Mesh->GetComponentRotation().Yaw;
	float ControllerYaw = PC->GetControlRotation().Yaw;

	float TextureYaw = MeshYaw - ControllerYaw;
	TextureYaw += 90.f;

	PlayerTexture->SetRenderTransformAngle(TextureYaw);
}

void USTMiniMapWidget::UpdateItemIcon(ASTPickupItem* Item, const FVector2D& MiniMapPos)
{
	if (!Item) return;

	UUserWidget** FoundWidget = ItemIconMap.Find(Item);
	UUserWidget* IconWidget = nullptr;

	if (!FoundWidget)
	{
		IconWidget = CreateWidget<USTMiniMapItemIconWidget>(GetWorld(), ItemIconClass);
		if (!IconWidget) return;

		ItemLayer->AddChild(IconWidget);
		ItemIconMap.Add(Item, IconWidget);
	}
	else
	{
		IconWidget = *FoundWidget;
	}

	IconWidget->SetRenderTranslation(MiniMapPos);
}

void USTMiniMapWidget::HideItemIcon(ASTPickupItem* Item)
{
	if (!Item) return;

	UUserWidget** FoundWidget = ItemIconMap.Find(Item);
	if (!FoundWidget) return;

	(*FoundWidget)->RemoveFromParent();

	ItemIconMap.Remove(Item);
}