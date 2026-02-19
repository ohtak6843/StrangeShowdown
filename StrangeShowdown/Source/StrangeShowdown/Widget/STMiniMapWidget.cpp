// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/STMiniMapWidget.h"
#include "Components/Image.h"

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

}

void USTMiniMapWidget::HideItemIcon(ASTPickupItem* Item)
{

}