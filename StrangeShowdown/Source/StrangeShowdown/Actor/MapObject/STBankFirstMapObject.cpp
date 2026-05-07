// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/MapObject/STBankFirstMapObject.h"
#include "Character/Player/STLocalPlayer.h"

void ASTBankFirstMapObject::ActivationMapObject(APawn* Interactor)
{
	// Interactor을 LocalPlayer로 캐스팅
	APlayerController* PC = Cast<APlayerController>(Interactor->GetController());
	if (PC)
	{
		ASTLocalPlayer* LocalPlayer = Cast<ASTLocalPlayer>(PC->GetPawn());
		if (LocalPlayer)
		{
			LocalPlayer->ActiveBankFirstMapObject();
		}
	}
}