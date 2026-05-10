// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Lobby/STLobbyStatusWidget.h"
#include "UI/ChatManager/STChatManagerWidget.h"
#include "Components/TextBlock.h"
#include "STLobbyHUD.generated.h"

/**
 * 
 */
UCLASS()
class STRANGESHOWDOWN_API USTLobbyHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USTChatManagerWidget> ChatManagerWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USTLobbyStatusWidget> LobbyStatusWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ReadyText;

	void SetReady(const uint64 InPlayerID, const bool InReady);
	void SetOtherPlayerReady(const uint64 InPlayerID, const bool InReady);
};
