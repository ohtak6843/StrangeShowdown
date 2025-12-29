// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/STFieldPlayer.h"

ASTFieldPlayer::ASTFieldPlayer()
{
	// Stat Widget Component
	StatWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatWidgetComponent"));
	StatWidgetComponent->SetupAttachment(RootComponent);
	StatWidgetComponent->SetWidgetSpace(EWidgetSpace::World);

}

void ASTFieldPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 위젯이 카메라를 바라보도록 처리
	if (StatWidgetComponent)
	{
		FVector CameraLocation;
		FRotator CameraRotation;

		// 플레이어 카메라 얻기
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC && PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->GetCameraViewPoint(CameraLocation, CameraRotation);

			// UI가 카메라를 바라보게
			FVector Direction = CameraLocation - StatWidgetComponent->GetComponentLocation();
			FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
			StatWidgetComponent->SetWorldRotation(LookAtRotation);
		}
	}
}