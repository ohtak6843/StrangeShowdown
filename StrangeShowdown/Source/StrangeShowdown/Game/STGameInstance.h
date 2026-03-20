// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/STRoomInfoObject.h"
#include "Engine/GameInstance.h"

#include "Containers/Map.h"
#include "Protocol.h"
#include "Character/Player/STPlayerBase.h"
#include "Character/Player/STFieldPlayer.h"


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

	void DisconnectFromGameServer();

	void HandleRecvPackets();

	bool GameInstanceTick(float DeltaTime);

	// --
	// packet handle
	// --

	// 정적
	void HandleSpawn(const Common::SCSpawnObject& Packet);
	void HandleMove(const Common::SCMovePlayer& Packet);
	//void HandleCreateRoom(const Common::SCCreateRoom& Packet);
	void HandleJoinRoom(const Common::SCJoinRoom& Packet);

	// 동적
	void HandleGiveRoomList(const Common::SCGiveRoomList& Packet, const uint8* PayloadPtr, const uint16 PayloadSize);



	// blueprint 명령어

	UFUNCTION(BlueprintCallable, Category = "Network")
	void GetRoomList();

	
	UFUNCTION(BlueprintCallable, Category = "Network")
	void JoinRoom(const int64 RoomID, const FText& Password = FText::GetEmpty());

	UFUNCTION(BlueprintCallable, Category = "Network")
	void CreateRoom(const FText& Name, const FText& Password);

	UFUNCTION(BlueprintCallable, Category = "Network")
	void ChangeWorld();


	// Exec 명령어

	UFUNCTION(Exec)
	void DevGetRoomList();

	UFUNCTION(Exec)
	void DevJoinRoom(const uint32 RoomID);

	UFUNCTION(Exec)
	void DevCreateRoom(const FString& RoomName, const FString& Password);

	UFUNCTION(Exec)
	void DevChangeWorld();


	// util

	void SendPacket(const TArray<uint8>& data);
	void OnLevelLoaded(UWorld* LoadedWorld);
	

public:
	// blueprint 다른 플레이어의 타입 지정
	UPROPERTY(EditAnywhere, Category = "SpawnData")
	TSubclassOf<ASTFieldPlayer> OtherPlayerClass{};

private:
	FSocket* Socket{};
	TSharedPtr<SocketIO> SocketIOInstance{};
	TSharedPtr<STPacketHandler> PacketHandler{};


	// 가비지 컬렉터 삭제 방지
	UPROPERTY(VisibleAnywhere, Category = "Network")
	TMap<uint64, ASTFieldPlayer*> PlayerMap{};

	// GameInstance 자체 Ticker
	FTSTicker::FDelegateHandle TickHandle{};

	// 레벨 로딩 중 여부
	bool IsLoadingLevel{ false };

};
