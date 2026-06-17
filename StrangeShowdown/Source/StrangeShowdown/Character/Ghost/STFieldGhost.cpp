// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ghost/STFieldGhost.h"

ASTFieldGhost::ASTFieldGhost()
{
}

void ASTFieldGhost::Tick(float DeltaTime)
{
#if NETWORK_ENABLED
	TickMove(DeltaTime);
#endif
}
