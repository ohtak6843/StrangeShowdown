#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "STLobbyStatusWidget.generated.h"

struct FPlayerLobbyData
{
	uint64 PlayerID;
	FString NickName;
	bool bReady;
	bool bIsHost;
};

UCLASS()
class STRANGESHOWDOWN_API USTLobbyStatusWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void EnterPlayer(uint64 PlayerID, const FString& NickName, bool bReady);
	void LeavePlayer(uint64 PlayerID);
	void RebuildSlots();
	void SetPlayerReady(uint64 PlayerID, bool bReady);

	// 슬롯 업데이트 (특정 플레이어 변경 시 호출)
	void UpdateSlot(const uint64 PlayerID);
	// 슬롯 업데이트 (플레이어 입장, 퇴장, 준비 상태 변경 시 호출)
	void UpdateSlot();

private:
	const int MaxPlayerCount = 5;

	TMap<uint64, FPlayerLobbyData> PlayerMap;
	TMap<uint64, int32> PlayerIDToSlotIndexMap;

	// 다음 슬롯 (1부터 시작)
	int32 NextSlotIndex = 1;

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* RoomOwnerNickName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* RoomOwnerReadyIcon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Player1NickName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Player1ReadyIcon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Player2NickName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Player2ReadyIcon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Player3NickName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Player3ReadyIcon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Player4NickName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* Player4ReadyIcon;
};