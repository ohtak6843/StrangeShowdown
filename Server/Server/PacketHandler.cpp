#include "pch.h"
#include "PacketHandler.h"
#include "RoomManager.h"
#include "IOCP.h"
#include "Serializer.h"

std::unordered_map<Common::PacketType, PacketHandler::HandlerFunc> PacketHandler::_handlerMap;

void PacketHandler::Init()
{
	// static 핸들러 등록

	RegisterHandler<Common::CSLogin>(
		Common::PacketType::CS_LOGIN,
		[](SessionPtr session, const auto& packet)
		{
			HandleLogin(session, packet);
		}
	);

	RegisterHandler<Common::CSMovePlayer>(
		Common::PacketType::CS_MOVE_PLAYER,
		[](SessionPtr session, const auto& packet)
		{
			HandleMovePlayer(session, packet);
		}
	);

	RegisterHandler<Common::CSGetRoomList>(
		Common::PacketType::CS_GET_ROOM_LIST,
		[](SessionPtr session, const auto& packet)
		{
			HandleGetRoomList(session, packet);
		}
	);

	RegisterHandler<Common::CSCreateRoom>(
		Common::PacketType::CS_CREATE_ROOM,
		[](SessionPtr session, const auto& packet)
		{
			HandleCreateRoom(session, packet);
		}
	);

	RegisterHandler<Common::CSJoinRoom>(
		Common::PacketType::CS_JOIN_ROOM,
		[](SessionPtr session, const auto& packet)
		{
			HandleJoinRoom(session, packet);
		}
	);

	RegisterHandler<Common::CSReady>(
		Common::PacketType::CS_READY,
		[](SessionPtr session, const auto& packet)
		{
			HandleReady(session, packet);
		}
	);

	RegisterHandler<Common::CSStartGame>(
		Common::PacketType::CS_START_GAME,
		[](SessionPtr session, const auto& packet)
		{
			HandleStartGame(session, packet);
		}
	);

	RegisterHandler<Common::CSUseItem>(
		Common::PacketType::CS_USE_ITEM,
		[](SessionPtr session, const auto& packet)
		{
			HandleUseItem(session, packet);
		}
	);

	RegisterHandler<Common::CSPickItem>(
		Common::PacketType::CS_PICK_ITEM,
		[](SessionPtr session, const auto& packet)
		{
			HandlePickItem(session, packet);
		}
	);

	// dynamic handler 등록

	RegisterDynamicHandler<Common::CSChat>(
		Common::PacketType::CS_CHAT,
		[](SessionPtr session, const auto& packet, const uint8* payload, const uint16 payload_size)
		{
			HandleChat(session, packet, payload, payload_size);
		}
	);

}

void PacketHandler::HandlePacket(SessionPtr session, const RecvBuffer& buffer)
{
	if (nullptr == session)
	{
		return;
	}

	Common::Header& header{ *reinterpret_cast<Common::Header*>(const_cast<uint8*>(buffer.data())) };
	auto type{ header.type };
	auto iter{ _handlerMap.find(type) };
	if (_handlerMap.end() != iter)
	{
		auto& handler_func{ iter->second };
		handler_func(session, buffer);
	}
}

void PacketHandler::HandleMovePlayer(SessionPtr session, const Common::CSMovePlayer& packet)
{
	if (auto player{ session->GetPlayer() }; nullptr != player)
	{
		player->HandleMove(packet);
	}
}

void PacketHandler::HandleLogin(SessionPtr session, const Common::CSLogin& packet)
{
	Common::SCLogin login_packet{};
	session->DoSend(login_packet);
}

void PacketHandler::HandleGetRoomList(SessionPtr session, const Common::CSGetRoomList& packet)
{
	GET_SINGLE(RoomManager)->HandleGetRoomList(session, packet);
}

void PacketHandler::HandleCreateRoom(SessionPtr session, const Common::CSCreateRoom& packet)
{
	GET_SINGLE(RoomManager)->HandleCreateRoom(session, packet);
}

void PacketHandler::HandleJoinRoom(SessionPtr session, const Common::CSJoinRoom& packet)
{
	GET_SINGLE(RoomManager)->HandleJoinRoom(session, packet);
}

void PacketHandler::HandleReady(SessionPtr session, const Common::CSReady& packet)
{
	if (auto room{ session->GetRoom() }; nullptr != room)
	{
		room->HandleReady(session, packet);
	}
}

void PacketHandler::HandleStartGame(SessionPtr session, const Common::CSStartGame& packet)
{
	if (auto room{ session->GetRoom() }; nullptr != room)
	{
		room->HandleStart(session);
	}
}

void PacketHandler::HandleUseItem(SessionPtr session, const Common::CSUseItem& packet)
{
	if (auto room{ session->GetRoom() }; nullptr != room)
	{
		room->HandleUseItem(session, packet);
	}
}

void PacketHandler::HandlePickItem(SessionPtr session, const Common::CSPickItem& packet)
{
	if (auto player{ session->GetPlayer() }; nullptr != player)
	{
		player->HandlePickItem(packet);
	}
}

void PacketHandler::HandleChat(SessionPtr session, const Common::CSChat& packet, const uint8* payload, const uint16 payload_size)
{
	if (auto room{ session->GetRoom() }; nullptr != room)
	{
		room->HandleChat(session, packet, payload, payload_size);
	}
}
