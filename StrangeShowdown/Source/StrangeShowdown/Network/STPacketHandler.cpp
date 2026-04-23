// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/STPacketHandler.h"

STPacketHandler::STPacketHandler()
{
	
	GameInstance = Cast<USTGameInstance>(GWorld->GetGameInstance());
	
	// 고정 크기 패킷 핸들러 등록
	RegisterHandler<Common::SCSpawnObject>(
		Common::PacketType::SC_SPAWN_OBJECT,
		[this](const auto& Packet)
		{
			HandleSpawnObject(Packet);
		}
	);

	RegisterHandler<Common::SCMovePlayer>(
		Common::PacketType::SC_MOVE_PLAYER,
		[this](const auto& Packet)
		{
			HandleMoveObject(Packet);
		}
	);


	RegisterHandler<Common::SCCreateRoom>(
		Common::PacketType::SC_CREATE_ROOM,
		[this](const auto& Packet)
		{
			HandleCreateRoom(Packet);
		}
	);

	RegisterHandler<Common::SCJoinRoom>(
		Common::PacketType::SC_JOIN_ROOM,
		[this](const auto& Packet)
		{
			HandleJoinRoom(Packet);
		}
	);

	RegisterHandler<Common::SCLogin>(
		Common::PacketType::SC_LOGIN,
		[this](const auto& Packet)
		{
			HandleLogin(Packet);
		}
	);

	// 가변 크기 핸들러 등록
	RegisterHandlerDynamic<Common::SCGiveRoomList>(
		Common::PacketType::SC_GIVE_ROOM_LIST,
		[this](const auto& Packet, const uint8* PayloadPtr, const uint16 PayloadSize)
		{
			HandleGiveRoomList(Packet, PayloadPtr, PayloadSize);
		}
	);

	RegisterHandlerDynamic<Common::SCChat>(
		Common::PacketType::SC_CHAT,
		[this](const auto& Packet, const uint8* PayloadPtr, const uint16 PayloadSize)
		{
			HandleChat(Packet, PayloadPtr, PayloadSize);
		}
	);

	RegisterHandler<Common::SCReady>(
		Common::PacketType::SC_READY,
		[this](const auto& Packet)
		{
			HandleReady(Packet);
		}
	);

	RegisterHandler<Common::SCStartGame>(
		Common::PacketType::SC_START_GAME,
		[this](const auto& Packet)
		{
			HandleStartGame(Packet);
		}
	);
}

STPacketHandler::~STPacketHandler()
{
}

void STPacketHandler::HandlePacket(const TArray<uint8>& Data)
{
	Common::Header& header{ *reinterpret_cast<Common::Header*>(const_cast<uint8*>(Data.GetData())) };
	auto Type{ static_cast<Common::PacketType>(header.type) };
	if (auto* Func{ HandlerMap.Find(Type) })
	{
		(*Func)(Data);
	}
}

void STPacketHandler::HandleSpawnObject(const Common::SCSpawnObject& Packet)
{
	if (nullptr == GameInstance)
	{
		return;
	}
	GameInstance->HandleSpawn(Packet);
	UE_LOG(LogTemp, Log, TEXT("Spawn Success"));
}

void STPacketHandler::HandleMoveObject(const Common::SCMovePlayer& Packet)
{
	if (nullptr == GameInstance)
	{
		return;
	}
	GameInstance->HandleMove(Packet);
}

void STPacketHandler::HandleGiveRoomList(const Common::SCGiveRoomList& Packet, const uint8* PayloadPtr, const uint16 PayloadSize)
{
	if (nullptr == GameInstance)
	{
		return;
	}
	GameInstance->HandleGiveRoomList(Packet, PayloadPtr, PayloadSize);
}

void STPacketHandler::HandleReady(const Common::SCReady& Packet)
{
	if (nullptr == GameInstance)
	{
		return;
	}
	GameInstance->HandleReady(Packet);
}

void STPacketHandler::HandleStartGame(const Common::SCStartGame& Packet)
{
	if (nullptr == GameInstance)
	{
		return;
	}
	GameInstance->HandleStartGame(Packet);
}

void STPacketHandler::HandleChat(const Common::SCChat& Packet, const uint8* PayloadPtr, const uint16 PayloadSize)
{
	if (nullptr == GameInstance)
	{
		return;
	}
	GameInstance->HandleChat(Packet, PayloadPtr, PayloadSize);	
}

void STPacketHandler::HandleCreateRoom(const Common::SCCreateRoom& Packet)
{
	if (nullptr == GameInstance)
	{
		return;
	}
	GameInstance->HandleCreateRoom(Packet);
}

void STPacketHandler::HandleJoinRoom(const Common::SCJoinRoom& Packet)
{
	if (nullptr == GameInstance)
	{
		return;
	}
	GameInstance->HandleJoinRoom(Packet);
	UE_LOG(LogTemp, Log, TEXT("Room Join Success. roomID: %s"), Packet.success ? TEXT("true") : TEXT("false"));
}

void STPacketHandler::HandleLogin(const Common::SCLogin& Packet)
{
	if (nullptr == GameInstance)
	{
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("Login Success"));
}
