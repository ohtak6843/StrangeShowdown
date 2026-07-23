// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Protocol.h"
#include "Network/STSerializer.h"
#include "Game/STGameInstance.h"

/**
 * Socket I/O 통신
 */
class STRANGESHOWDOWN_API STPacketHandler
{
public:
	STPacketHandler();
	~STPacketHandler();

	void HandlePacket(const TArray<uint8>& Data);

private:

	// 고정 크기 핸들러
	void HandleSpawnPlayer(const Common::SCSpawnPlayer& Packet);
	void HandleDespawnPlayer(const Common::SCDespawnPlayer& Packet);
	void HandleSpawnObject(const Common::SCSpawnObject& Packet);
	void HandleDespawnObject(const Common::SCDespawnObject& Packet);
	void HandleMoveObject(const Common::SCMovePlayer& Packet);
	void HandleCreateRoom(const Common::SCCreateRoom& Packet);
	void HandleJoinRoom(const Common::SCJoinRoom& Packet);
	void HandleLogin(const Common::SCLogin& Packet);
	void HandleReady(const Common::SCReady& Packet);
	void HandleStartGame(const Common::SCStartGame& Packet);
	void HandleUseItem(const Common::SCUseItem& Packet);
	void HandleStatusUpdate(const Common::SCStatusUpdate& Packet);

	// 가변 크기 핸들러
	void HandleGiveRoomList(const Common::SCGiveRoomList& Packet, const uint8* PayloadPtr, const uint16 PayloadSize);
	void HandleChat(const Common::SCChat& Packet, const uint8* PayloadPtr, const uint16 PayloadSize);


	// 핸들러 맵에 함수 등록
	template <typename T>
	void RegisterHandler(Common::PacketType Type, TFunction<void(const T&)> LogicFunc)
	{
		HandlerMap.Add(
			Type,
			[this, LogicFunc](const TArray<uint8>& Data)
			{
				if (nullptr == GameInstance)
				{
					return;
				}

				T Packet{ STSerializer::Deserialize<T>(Data) };
				LogicFunc(Packet);
			}
		);
	}

	// 동적 크기 패킷을 처리하는 핸들러 등록
	template <typename T>
	void RegisterHandlerDynamic(Common::PacketType Type, TFunction<void(const T&, const uint8*, const uint16)> LogicFunc)
	{
		HandlerMap.Add(
			Type,
			[this, LogicFunc](const TArray<uint8>& Data)
			{
				// 방어: 버퍼 크기가 최소 헤더 크기도 안 되면 무시
				const uint16 PacketSize{ sizeof(T) };
				if (Data.Num() < sizeof(T))
				{
					return;
				}
				if (nullptr == GameInstance)
				{
					return;
				}

				// 고정 데이터 파싱
				T Packet{ STSerializer::Deserialize<T>(Data) };
				const uint16 TotalSize{ Packet.size };

				// 가변 데이터 파싱
				const uint8* PayloadPtr{ Data.GetData() + PacketSize };
				const uint16 PayloadSize{ static_cast<uint16>(TotalSize - PacketSize) };

				LogicFunc(Packet, PayloadPtr, PayloadSize);
			}
		);
	}

private:

	// 현재 인스턴스
	UPROPERTY()
	TObjectPtr<USTGameInstance> GameInstance;

	// 핸들러 함수 맵
	using FPacketHandlerFunc = TFunction<void(const TArray<uint8>&)>;
	TMap<Common::PacketType, FPacketHandlerFunc> HandlerMap;

};