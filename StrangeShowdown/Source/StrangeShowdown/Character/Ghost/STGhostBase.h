// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/STCharacter.h"
#include "STGhostBase.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API ASTGhostBase : public ASTCharacter
{
	GENERATED_BODY()

public:
	ASTGhostBase();

protected:

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> StaticMesh;
	
};
