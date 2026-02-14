// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/STRoomInfoObject.h"
#include "Engine/GameInstance.h"

#include "Containers/Map.h"
#include "StrangeShowdown.h"
#include "Player/STPlayerBase.h"
#include "Player/STFieldPlayer.h"


#include "STGameInstance.generated.h"

class SocketIO;
class STPacketHandler;

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoomListUpdated);

UCLASS()
class STRANGESHOWDOWN_API USTGameInstance : public UGameInstance
{
	GENERATED_BODY()

	// client
public:
	// 방 목록이 업데이트되었음을 알리는 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnRoomListUpdated OnRoomListUpdated;

public:
	// 방 목록을 저장하는 배열
	UPROPERTY(BlueprintReadWrite)
	TArray<USTRoomInfoObject*> RoomList;

	// 해상도
	UPROPERTY(BlueprintReadWrite)
	FIntPoint ScreenResolution;

	// 창모드
	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EWindowMode::Type> WindowMode = EWindowMode::Windowed;
	
	// 볼륨 설정
	UPROPERTY(BlueprintReadWrite)
	float MasterVolume = 1.0f;

	UPROPERTY(BlueprintReadWrite)
	float BGMVolume = 1.0f;

	UPROPERTY(BlueprintReadWrite)
	float SFXVolume = 1.0f;


public:
	virtual void Init() override;
	virtual void Shutdown() override;

	// 새로운 방을 추가
	void AddRoom(USTRoomInfoObject* NewRoom);


	// network
public:
	// blueprint

	UFUNCTION(BlueprintCallable)
	void ConnectToGameServer();

	UFUNCTION(BlueprintCallable)
	void DisconnectFromGameServer();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	// c++ method
	// packet handle


	void HandleSpawn(const Common::SCSpawnObject& Packet);
	void HandleMove(const Common::SCMovePlayer& Packet);
	
	UFUNCTION(BlueprintCallable)
	void TempFunc();

	// util
	void SendPacket(const TArray<uint8>& data);

public:
	// blueprint 다른 플레이어의 타입 지정
	UPROPERTY(EditAnywhere, Category = "SpawnData")
	TSubclassOf<ASTFieldPlayer> OtherPlayerClass;

private:
	FSocket* Socket{};
	TSharedPtr<SocketIO> SocketIOInstance{};
	TSharedPtr<STPacketHandler> PacketHandler{};
	TMap<uint64, ASTFieldPlayer*> PlayerMap{};

};
