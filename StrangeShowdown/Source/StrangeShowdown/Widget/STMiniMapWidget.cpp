// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/STMiniMapWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Item/STPickupItem.h"
#include "Actor/STMineral.h"

void USTMiniMapWidget::NativeConstruct()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	APawn* Pawn = PC->GetPawn();
	if (!Pawn) return;

	USkeletalMeshComponent* Mesh =
		Pawn->FindComponentByClass<USkeletalMeshComponent>();
	if (!Mesh) return;

	PlayerController = PC;
	PlayerPawn = Pawn;
	PlayerMesh = Mesh;
}

void USTMiniMapWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (!PlayerTexture) return;
	if (!PlayerController) return;
	if (!PlayerPawn) return;
	if (!PlayerMesh) return;

	float MeshYaw = PlayerMesh->GetComponentRotation().Yaw;
	float ControllerYaw = PlayerController->GetControlRotation().Yaw;

	if (IsRotationAble)
	{
		float TextureYaw = MeshYaw - ControllerYaw;
		TextureYaw += 90.f;
		PlayerTexture->SetRenderTransformAngle(TextureYaw);
	}
	else
	{
		float TextureYaw = MeshYaw;
		TextureYaw += 90.f;
		PlayerTexture->SetRenderTransformAngle(TextureYaw);
	}
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

void USTMiniMapWidget::UpdateMineralIcon(ASTMineral* Mineral, const FVector2D& MiniMapPos)
{
	if (!Mineral) return;

	UUserWidget** FoundWidget = MineralIconMap.Find(Mineral);
	UUserWidget* IconWidget = nullptr;

	if (!FoundWidget)
	{
		IconWidget = CreateWidget<USTMiniMapItemIconWidget>(GetWorld(), MineralIconClass);
		if (!IconWidget) return;

		MineralLayer->AddChild(IconWidget);
		MineralIconMap.Add(Mineral, IconWidget);
	}
	else
	{
		IconWidget = *FoundWidget;
	}

	IconWidget->SetRenderTranslation(MiniMapPos);
}

void USTMiniMapWidget::HideMineralIcon(ASTMineral* Mineral)
{
	if (!Mineral) return;

	UUserWidget** FoundWidget = MineralIconMap.Find(Mineral);
	if (!FoundWidget) return;

	(*FoundWidget)->RemoveFromParent();

	MineralIconMap.Remove(Mineral);
}

void USTMiniMapWidget::UpdatePlayerIcon(const FVector2D& MiniMapPos)
{
	PlayerTexture->SetRenderTranslation(MiniMapPos);
}