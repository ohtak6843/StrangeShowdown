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
		// 아이콘 위젯 생성
		IconWidget = CreateWidget<USTMiniMapItemIconWidget>(GetWorld(), ItemIconClass);

		if (!IconWidget) return;

		// 아이콘을 캔버스에 추가
		ItemLayer->AddChild(IconWidget);

		// 아이템과 아이콘 위젯을 맵에 저장
		ItemIconMap.Add(Item, IconWidget);
	}
	else
	{
		// 이미 아이콘이 존재하면 위치 업데이트
		IconWidget = *FoundWidget;
	}

	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(IconWidget->Slot))
	{
		// 아이콘 위치 설정
		CanvasSlot->SetPosition(MiniMapPos);
	}
}

void USTMiniMapWidget::HideItemIcon(ASTPickupItem* Item)
{
	if (!Item) return;

	UUserWidget** FoundWidget = ItemIconMap.Find(Item);
	if (!FoundWidget) return;

	(*FoundWidget)->RemoveFromParent();

	ItemIconMap.Remove(Item);
}