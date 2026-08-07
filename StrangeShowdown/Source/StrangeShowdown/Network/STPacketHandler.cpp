// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/STPacketHandler.h"

STPacketHandler::STPacketHandler()
{
	
	GameInstance = Cast<USTGameInstance>(GWorld->GetGameInstance());
	
	// 고정 크기 패킷 핸들러 등록
	RegisterHandler<Common::SCSpawnPlayer>(
		Common::PacketType::SC_SPAWN_PLAYER,
		[this](const auto& Packet)
		{
			HandleSpawnPlayer(Packet);
		}
	);

	RegisterHandler<Common::SCDespawnPlayer>(
		Common::PacketType::SC_DESPAWN_PLAYER,
		[this](const auto& Packet)
		{
			HandleDespawnPlayer(Packet);
		}
	);

	RegisterHandler<Common::SCSpawnObject>(
		Common::PacketType::SC_SPAWN_OBJECT,
		[this](const auto& Packet)
		{
			HandleSpawnObject(Packet);
		}
	);

	RegisterHandler<Common::SCDespawnObject>(
		Common::PacketType::SC_DESPAWN_OBJECT,
		[this](const auto& Packet)
		{
			HandleDespawnObject(Packet);
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

	RegisterHandler<Common::SCUseItem>(
		Common::PacketType::SC_USE_ITEM,
		[this](const auto& Packet)
		{
			HandleUseItem(Packet);
		}
	);

	RegisterHandler<Common::SCStatusUpdate>(
		Common::PacketType::SC_STATUS_UPDATE,
		[this](const auto& Packet)
		{
			HandleStatusUpdate(Packet);
		}
	);

	RegisterHandler<Common::SCSetTurn>(
		Common::PacketType::SC_SET_TURN,
		[this](const auto& Packet)
		{
			HandleSetTurn(Packet);
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

void STPacketHandler::HandleSpawnPlayer(const Common::SCSpawnPlayer& Packet)
{
	GameInstance->HandleSpawnPlayer(Packet);
}

void STPacketHandler::HandleDespawnPlayer(const Common::SCDespawnPlayer& Packet)
{
	GameInstance->HandleDespawnPlayer(Packet);
}

void STPacketHandler::HandleSpawnObject(const Common::SCSpawnObject& Packet)
{
	GameInstance->HandleSpawnObject(Packet);
}

void STPacketHandler::HandleDespawnObject(const Common::SCDespawnObject& Packet)
{
	GameInstance->HandleDespawnObject(Packet);
}

void STPacketHandler::HandleMoveObject(const Common::SCMovePlayer& Packet)
{
	GameInstance->HandleMove(Packet);
}

void STPacketHandler::HandleGiveRoomList(const Common::SCGiveRoomList& Packet, const uint8* PayloadPtr, const uint16 PayloadSize)
{
	GameInstance->HandleGiveRoomList(Packet, PayloadPtr, PayloadSize);
}

void STPacketHandler::HandleReady(const Common::SCReady& Packet)
{
	GameInstance->HandleReady(Packet);
}

void STPacketHandler::HandleStartGame(const Common::SCStartGame& Packet)
{
	GameInstance->HandleStartGame(Packet);
}

void STPacketHandler::HandleChat(const Common::SCChat& Packet, const uint8* PayloadPtr, const uint16 PayloadSize)
{
	GameInstance->HandleChat(Packet, PayloadPtr, PayloadSize);	
}

void STPacketHandler::HandleCreateRoom(const Common::SCCreateRoom& Packet)
{
	GameInstance->HandleCreateRoom(Packet);
}

void STPacketHandler::HandleJoinRoom(const Common::SCJoinRoom& Packet)
{
	GameInstance->HandleJoinRoom(Packet);
}

void STPacketHandler::HandleLogin(const Common::SCLogin& Packet)
{
	UE_LOG(LogTemp, Log, TEXT("Login Success"));
}

void STPacketHandler::HandleUseItem(const Common::SCUseItem& Packet)
{
	GameInstance->HandleUseItem(Packet);
}

void STPacketHandler::HandleStatusUpdate(const Common::SCStatusUpdate& Packet)
{
	GameInstance->HandleStatusUpdate(Packet);
}

void STPacketHandler::HandleSetTurn(const Common::SCSetTurn& Packet)
{
	if (nullptr != GameInstance)
	{
		GameInstance->HandleSetTurn(Packet);
	}
}