// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BountyPoster/STBountyPosterSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Character/Player/STPlayerBase.h"
#include "Component/STStatComponent.h"

USTBountyPosterSlotWidget::USTBountyPosterSlotWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Player2DTextures.Add(TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/StrangeShowdown/UI/Texture/BountyPosterPlayerTexture/T_BadGuy.T_BadGuy"))));
	Player2DTextures.Add(TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/StrangeShowdown/UI/Texture/BountyPosterPlayerTexture/T_BusinessMan.T_BusinessMan"))));
	Player2DTextures.Add(TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/StrangeShowdown/UI/Texture/BountyPosterPlayerTexture/T_CowBoy.T_CowBoy"))));
	Player2DTextures.Add(TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/StrangeShowdown/UI/Texture/BountyPosterPlayerTexture/T_CowGirl.T_CowGirl"))));
	Player2DTextures.Add(TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/StrangeShowdown/UI/Texture/BountyPosterPlayerTexture/T_GunMan.T_GunMan"))));
	Player2DTextures.Add(TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/StrangeShowdown/UI/Texture/BountyPosterPlayerTexture/T_Woman.T_Woman"))));
	Player2DTextures.Add(TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/StrangeShowdown/UI/Texture/BountyPosterPlayerTexture/T_WorkingGirl.T_WorkingGirl"))));
}

void USTBountyPosterSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	int32 StaminaCount = 5;
	for (int32 i = 0; i < StaminaCount; i++)
	{
		UImage* StaminaImage = Cast<UImage>(GetWidgetFromName(FName(*FString::Printf(TEXT("StaminaImage%d"), i))));
		ensure(StaminaImage);
		StaminaImages.Add(StaminaImage);
	}
}

void USTBountyPosterSlotWidget::UpdateBountyPosterSlot(ASTPlayerBase* InPlayer)
{
	if (InPlayer)
	{
		EPlayerMeshType PlayerMeshType = InPlayer->GetPlayerMeshType();
		FSTCharacterStat& CharacterStat = InPlayer->StatComp->GetCharacterStat();

		PlayerBountyText->SetText(FText::AsNumber(CharacterStat.Bounty));
		PlayerNameText->SetText(FText::FromString(InPlayer->PlayerNickName));

		UTexture2D* Texture = nullptr;

		switch(PlayerMeshType)
		{
			case EPlayerMeshType::Badguy:
				Texture = Player2DTextures[0].LoadSynchronous();
				break;

			case EPlayerMeshType::BusinessMan:
				Texture = Player2DTextures[1].LoadSynchronous();
				break;

			case EPlayerMeshType::Cowboy:
				Texture = Player2DTextures[2].LoadSynchronous();
				break;

			case EPlayerMeshType::Cowgirl:
				Texture = Player2DTextures[3].LoadSynchronous();
				break;

			case EPlayerMeshType::Gunman:
				Texture = Player2DTextures[4].LoadSynchronous();
				break;

			case EPlayerMeshType::Woman:
				Texture = Player2DTextures[5].LoadSynchronous();
				break;

			case EPlayerMeshType::WorkingGirl:
				Texture = Player2DTextures[6].LoadSynchronous();
				break;
		}

		if (Texture)
		{
			PlayerImage->SetBrushFromTexture(Texture);
		}

		PlayerGold->SetText(FText::AsNumber(CharacterStat.CurrentGold));

		PlayerHpBar->SetPercent(CharacterStat.CurrentHp / CharacterStat.MaxHp);
		FString HpString = FString::Printf(TEXT("%.0f / %.0f"), CharacterStat.CurrentHp, CharacterStat.MaxHp);
		PlayerHpText->SetText(FText::FromString(HpString));

		for (int i = 0; i < StaminaImages.Num(); i++)
		{
			if (i < FMath::FloorToInt(CharacterStat.CurrentStamina))
			{
				StaminaImages[i]->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				StaminaImages[i]->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}
