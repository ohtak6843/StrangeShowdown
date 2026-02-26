// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Sheriff/STSheriffBase.h"

ASTSheriffBase::ASTSheriffBase()
{
	// Set Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/CharactersUE4Mannequin/SK_Chr_Sheriff_01.SK_Chr_Sheriff_01'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	// TODO: Set Anim Instance
	//static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/ArenaBattle/Animation/ABP_ABCharacter.ABP_ABCharacter_C"));
	//if (AnimInstanceClassRef.Class)
	//{
	//	GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	//}
}
