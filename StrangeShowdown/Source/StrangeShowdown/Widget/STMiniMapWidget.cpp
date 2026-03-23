#include "Widget/STMiniMapWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interface/STMiniMapTargetInterface.h"
#include "Widget/STMiniMapItemIconWidget.h"

void USTMiniMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	APawn* Pawn = PC->GetPawn();
	if (!Pawn) return;

	USkeletalMeshComponent* Mesh = Pawn->FindComponentByClass<USkeletalMeshComponent>();
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
		float TextureYaw = MeshYaw + 90.f;
		PlayerTexture->SetRenderTransformAngle(TextureYaw);
	}
}

void USTMiniMapWidget::UpdateTargetIcon(AActor* TargetActor, const FVector2D& MiniMapPos)
{
	if (!TargetActor) return;
	if (!TargetActor->Implements<USTMiniMapTargetInterface>()) return;
	if (!IconLayer) return;
	if (!IconClass) return;

	USTMiniMapItemIconWidget* IconWidget = nullptr;

	UUserWidget** FoundWidget = IconMap.Find(TargetActor);

	if (!FoundWidget)
	{
		IconWidget = CreateWidget<USTMiniMapItemIconWidget>(GetWorld(), IconClass);
		if (!IconWidget) return;

		UTexture2D* IconTexture =
			ISTMiniMapTargetInterface::Execute_GetMiniMapIcon(TargetActor);

		if (IconTexture)
		{
			IconWidget->SetIconTexture(IconTexture);
		}

		IconWidget->SetVisibility(ESlateVisibility::Hidden);

		IconLayer->AddChild(IconWidget);
		IconMap.Add(TargetActor, IconWidget);
	}
	else
	{
		IconWidget = Cast<USTMiniMapItemIconWidget>(*FoundWidget);
	}

	if (!IconWidget) return;

	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(IconWidget->Slot))
	{
		CanvasSlot->SetPosition(MiniMapPos);
	}
	else
	{
		IconWidget->SetRenderTranslation(MiniMapPos);
	}

	if (IconWidget->GetVisibility() != ESlateVisibility::HitTestInvisible)
	{
		IconWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void USTMiniMapWidget::HideTargetIcon(AActor* TargetActor)
{
	if (!TargetActor) return;

	UUserWidget** FoundWidget = IconMap.Find(TargetActor);
	if (!FoundWidget) return;

	UUserWidget* Widget = *FoundWidget;
	if (!Widget) return;

	if (Widget->GetVisibility() != ESlateVisibility::Hidden)
	{
		Widget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void USTMiniMapWidget::UpdatePlayerIcon(const FVector2D& MiniMapPos)
{
	if (!PlayerTexture) return;

	PlayerTexture->SetRenderTranslation(MiniMapPos);
}