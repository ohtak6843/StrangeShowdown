#include "pch.h"
#include "PacketHandler.h"
#include "RoomManager.h"
#include "IOCP.h"
#include "Serializer.h"

std::unordered_map<Common::PacketType, PacketHandler::HandlerFunc> PacketHandler::_handlerMap;

void PacketHandler::Init()
{
	// 핸들러 등록
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
}

void PacketHandler::HandlePacket(SessionPtr session, const RecvBuffer& buffer)
{
	if (nullptr == session)
	{
		return;
	}

	Common::Header& header{ *reinterpret_cast<Common::Header*>(const_cast<int8*>(buffer.data())) };
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
	auto id{ session->GetSessionID() };
	if (IOState::INGAME != session->GetIOState())
	{
		return;
	}


	auto room{ session->GetRoom()};
	if (nullptr == room)
	{
		return;
	}

	const auto& player_map{ room->GetPlayers() };
	auto res{ player_map.find(id) };
	if (res == player_map.end())
	{
		return;
	}

	auto& player{ res->second };

	player->HandleMove(packet);

	 
	for (const auto& [other_id, other_player] : player_map)
	{
		if (other_id == id)
		{
			continue;
		}

		// 다른 플레이어에게 내 위치 전달
		Common::SCMovePlayer move_object_packet{
			id,
			player->GetPosition(),
			player->GetDirection(),
			player->GetState()
		};
		other_player->GetOwnerSession()->DoSend(move_object_packet);
	}
}

void PacketHandler::HandleLogin(SessionPtr session, const Common::CSLogin& packet)
{
	auto id{ session->GetSessionID() };
	Common::SCLogin login_packet{};
	session->DoSend(login_packet);
}

void PacketHandler::HandleGetRoomList(SessionPtr session, const Common::CSGetRoomList& packet)
{
	auto room_list{ GET_SINGLE(RoomManager)->GetRoomList() };

	Common::SCGiveRoomList room_list_packet{
		static_cast<uint16>(room_list.size())
	};
	room_list_packet.size += static_cast<uint16>(sizeof(Common::RoomInfo) * room_list.size());

	// sendbuffer에 패킷과 방 리스트를 직접 직렬화해서 보냄.
	auto buffer{ Serializer::Serialize(room_list_packet, room_list) };
	session->DoSend(buffer);
}

void PacketHandler::HandleCreateRoom(SessionPtr session, const Common::CSCreateRoom& packet)
{
	// 방을 하나 만든다.
	uint32 room_id{};
	auto res{ GET_SINGLE(RoomManager)->CreateRoom(room_id) };

	// 방을 성공적으로 만들었으면 패킷을 보내준다
	auto buffer{ Serializer::Serialize(Common::SCCreateRoom{ res }) };

	if (false == res)
	{
		return;
	}

	// HandleJoinRoom을 통해 방에 입장한다.
	HandleJoinRoom(session, Common::CSJoinRoom{ room_id });
}

void PacketHandler::HandleJoinRoom(SessionPtr session, const Common::CSJoinRoom& packet)
{
	GET_SINGLE(RoomManager)->JoinRoom(session, packet.roomID);
}
