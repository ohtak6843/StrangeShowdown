// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/STGameMode.h"

ASTGameMode::ASTGameMode()
{
	// DefaultPawnClass
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Game/StrangeShowdown/Blueprint/BP_LocalPlayerBase.BP_LocalPlayerBase_C"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	// PlayerControllerClass
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Game/StrangeShowdown/Blueprint/PC_PlayerController.PC_PlayerController_C"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
}
