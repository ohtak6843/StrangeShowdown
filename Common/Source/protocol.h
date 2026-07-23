#pragma once

#include "BasicStruct.h"
#include "CommonNamespace.h"
#include "CommonDefine.h"
#include "CommonEnum.h"

COMMON_START

enum class PacketType : uint16
{
	NONE = 0,

	// --
	// prepare
	// --

	SC_LOGIN,
	CS_LOGIN,

	// --
	// room
	// --

	SC_GIVE_ROOM_LIST,
	CS_GET_ROOM_LIST,

	SC_CREATE_ROOM,
	CS_CREATE_ROOM,

	SC_JOIN_ROOM,
	CS_JOIN_ROOM,

	// -- 
	// lobby
	// --

	SC_READY,
	CS_READY,

	SC_START_GAME,
	CS_START_GAME,

	SC_CHAT,
	CS_CHAT,

	// --
	// object
	// --

	SC_SPAWN_PLAYER,
	SC_DESPAWN_PLAYER,

	SC_SPAWN_OBJECT,
	SC_DESPAWN_OBJECT,

	SC_MOVE_OBJECT,

	SC_MOVE_PLAYER,
	CS_MOVE_PLAYER,

	SC_TELEPORT,

	// --
	// content
	// --

	CS_PICK_ITEM,

	SC_USE_ITEM,
	CS_USE_ITEM,


	SC_DAMAGE,
	SC_STATUS_UPDATE,
	

};


#pragma pack(push, 1)

// 전송용 구조체

// 방 정보
struct RoomInfo
{
	uint32 roomId{};
	uint8 currentPlayerCount{};
	bool hasPassword{};
	char name[100]{};
};

// 패킷 앞에 공통으로 붙는 헤더
struct Header
{
	uint16		size{ sizeof(Header) };
	PacketType	type{ PacketType::NONE };

	Header() = default;
	Header(const uint16 in_size, const PacketType in_type) :
		size{ in_size },
		type{ in_type }
	{
	}
};

// Brief:
//  서버 로그인 승인 패킷
struct SCLogin : Header
{
	SCLogin() :
		Header{ sizeof(SCLogin), PacketType::SC_LOGIN }
	{
	}
};


// No Param
// Brief:
//  클라이언트 로그인 요청
struct CSLogin : Header
{
	CSLogin() :
		Header{ sizeof(CSLogin), PacketType::CS_LOGIN }
	{
	}
};

// SCGiveRoomList
// Param:
//	uint16 roomCount
// Brief:
//	[ 가변 크기 패킷 ]
//	서버 방 리스트 제공
//	방의 갯수만큼 크기를 조절해 RoomInfo를 이어 보낸다.
struct SCGiveRoomList : Header
{
	uint16 roomCount{};

	SCGiveRoomList() = default;
	SCGiveRoomList(const uint16 in_room_count) :
		Header{ sizeof(SCGiveRoomList), PacketType::SC_GIVE_ROOM_LIST },
		roomCount{ in_room_count }
	{
	}
};

// No Param
// Brief:
//  클라이언트 방 리스트 제공 요청
struct CSGetRoomList : Header
{
	CSGetRoomList() :
		Header{ sizeof(CSGetRoomList), PacketType::CS_GET_ROOM_LIST }
	{
	}
};


// Param:
//  bool success
// Brief:
//  서버 방 생성 여부
struct SCCreateRoom : Header
{
	bool success{ false };
	SCCreateRoom() = default;
	SCCreateRoom(const bool in_success) :
		Header{ sizeof(SCCreateRoom), PacketType::SC_CREATE_ROOM },
		success{ in_success }
	{
	}
};


// CSCreateRoom
// No Param
// Brief:
//  클라이언트 방 생성 요청
struct CSCreateRoom : Header
{
	char name[100]{};
	bool hasPassword{ false };
	char password[21]{};

	CSCreateRoom() = default;
	CSCreateRoom(const char* in_name, bool in_hasPassword, const char* in_password = "") :
		Header{ sizeof(CSCreateRoom), PacketType::CS_CREATE_ROOM },
		hasPassword{ in_hasPassword }
	{
		strncpy_s(name, in_name, sizeof(name) - 1);
		if (in_hasPassword && in_password != nullptr)
		{
			strncpy_s(password, in_password, sizeof(password) - 1);
		}
	}
};

// SCJoinRoom
// Param:
//  bool success
//  uint64 hostID
// Brief:
//  서버 방 입장 승인 및 방 정보 제공
struct SCJoinRoom : Header
{
	bool success{ false };
	uint64 hostID{};
	uint64 MyID{};
	SCJoinRoom() = default;
	SCJoinRoom(const bool in_success, const uint64 in_hostID = 0, const uint64 in_myID = 0) :
		Header{ sizeof(SCJoinRoom), PacketType::SC_JOIN_ROOM },
		success{ in_success },
		hostID{ in_hostID },
		MyID{ in_myID }
	{
	}
};

// CSJoinRoom
struct CSJoinRoom : Header
{
	uint32 roomID{};
	char password[21]{};
	CSJoinRoom() = default;
	CSJoinRoom(const uint32 in_roomID, const char* in_password = "") :
		Header{ sizeof(CSJoinRoom), PacketType::CS_JOIN_ROOM },
		roomID{ in_roomID }
	{
		if (in_password != nullptr)
		{
			strncpy_s(password, in_password, sizeof(password) - 1);
		}
	}
};

// SCReady
// Param:
//  uint64 id
//  bool ready
struct SCReady : Header
{
	uint64 id{};
	bool ready{ false };
	SCReady() = default;
	SCReady(const uint64 in_id, const bool in_ready) :
		Header{ sizeof(SCReady), PacketType::SC_READY },
		id{ in_id },
		ready{ in_ready }
	{
	}
};


// CSReady
// Param:
//  bool ready
// 
struct CSReady : Header
{
	bool ready{ false };
	CSReady() = default;
	CSReady(const bool in_ready) :
		Header{ sizeof(CSReady), PacketType::CS_READY },
		ready{ in_ready }
	{
	}
};


// SCStartGame
// param:
//  bool start
// brief:
//  게임 시작 여부를 알리는 패킷. start가 true면 게임이 시작되고 false면 게임이 취소됨.
struct SCStartGame : Header
{
	bool start{ false };
	SCStartGame() = default;
	SCStartGame(const bool in_start) :
		Header{ sizeof(SCStartGame), PacketType::SC_START_GAME },
		start{ in_start }
	{
	}
};

// CSStartGame
// No Param
struct CSStartGame : Header
{
	CSStartGame() :
		Header{ sizeof(CSStartGame), PacketType::CS_START_GAME }
	{
	}
};

// SCChat
// Param:
//  uint64 보낸 사람의 id
// Brief:
//  [ 가변 크기 패킷 ]
//  각 클라이언트에 채팅 메시지를 전달하는 패킷. 채팅 메시지를 뒤에 이어붙인다.
struct SCChat : Header
{
	uint64 id{};
	SCChat() = default;
	SCChat(const uint64 in_id, const uint16 in_size) :
		Header{ static_cast<uint16>(sizeof(SCChat) + in_size), PacketType::SC_CHAT },
		id{ in_id }
	{
	}
};

// CSChat
//  no param
// Brief:
//  [ 가변 크기 패킷 ]
//  서버로 채팅 메시지를 보내는 패킷. 채팅 메시지를 뒤에 이어붙인다.
struct CSChat : Header
{
	CSChat() = default;
	CSChat(const uint16 in_size) :
		Header{ static_cast<uint16>(sizeof(CSChat) + in_size), PacketType::CS_CHAT }
	{
	}
};

// Param:
//  uint64 objectID
//  Vec3f pos
//  Vec3f dir
//  PlayerType type
//	
// Brief:
//  클라이언트 로그인 요청
struct SCSpawnPlayer : Header
{
	uint64 id{};
	Vec3f pos{};
	Vec3f dir{};
	PlayerType type{ PlayerType::None };

	SCSpawnPlayer() = default;
	SCSpawnPlayer(const uint64 in_id, const Vec3f& in_pos, const Vec3f& in_dir, const PlayerType in_type = PlayerType::None) :
		Header{ sizeof(SCSpawnPlayer), PacketType::SC_SPAWN_PLAYER },
		id{ in_id },
		pos{ in_pos },
		dir{ in_dir },
		type{ in_type } 
	{
	}
};

struct SCDespawnPlayer : Header
{
	uint64 id{};
	SCDespawnPlayer() = default;
	SCDespawnPlayer(const uint64 in_id) :
		Header{ sizeof(SCDespawnPlayer), PacketType::SC_DESPAWN_PLAYER },
		id{ in_id }
	{
	}
};

// SCSpawnObject
struct SCSpawnObject : Header
{
	uint32 id{};
	Vec3f pos{};
	Vec3f dir{};
	ObjectType type{ ObjectType::None };
	SCSpawnObject() = default;
	SCSpawnObject(const uint32 in_id, const Vec3f& in_pos, const Vec3f& in_dir, const ObjectType in_type = ObjectType::None) :
		Header{ sizeof(SCSpawnObject), PacketType::SC_SPAWN_OBJECT },
		id{ in_id },
		pos{ in_pos },
		dir{ in_dir },
		type{ in_type }
	{
	}
};

struct SCDespawnObject : Header
{
	uint32 id{};
	SCDespawnObject() = default;
	SCDespawnObject(const uint32 in_id) :
		Header{ sizeof(SCDespawnObject), PacketType::SC_DESPAWN_OBJECT },
		id{ in_id }
	{
	}
};

// Param:
//  uint64 id
//  Vec3f pos
//  Vec3f dir
struct SCMovePlayer : Header
{
	uint64 id{};
	Vec3f pos{};
	Vec3f dir{};
	uint8 state{};

	SCMovePlayer() = default;
	SCMovePlayer(const uint64 in_id, const Vec3f& in_pos, const Vec3f& in_dir, const uint8 in_state) :
		Header{ sizeof(SCMovePlayer), PacketType::SC_MOVE_PLAYER },
		id{ in_id },
		pos{ in_pos },
		dir{ in_dir },
		state{ in_state }
	{
	}
};

// Param:
//  Vec3f pos
//  Vec3f dir
//  uint8 state
// 
struct CSMovePlayer : Header
{
	Vec3f pos{};
	Vec3f dir{};
	uint8 state{};

	CSMovePlayer() = default;
	CSMovePlayer(const Vec3f& in_pos, const Vec3f& in_dir, const uint8 in_state) :
		Header{ sizeof(CSMovePlayer), PacketType::CS_MOVE_PLAYER },
		pos{ in_pos },
		dir{ in_dir },
		state{ in_state }
	{
	}
};


// Param:
//  uint64 id
//  Vec3f pos
struct SCTeleport: Header
{
	uint64 id{};
	Vec3f pos{};
	SCTeleport() = default;
	SCTeleport(const uint64 in_id, const Vec3f& in_pos) :
		Header{ sizeof(SCTeleport), PacketType::SC_TELEPORT },
		id{ in_id },
		pos{ in_pos }
	{
	}
};

// pick item
struct CSPickItem : Header
{
	Common::ItemType itemType{};
	CSPickItem() = default;
	CSPickItem(const Common::ItemType in_itemType) :
		Header{ sizeof(CSPickItem), PacketType::CS_PICK_ITEM },
		itemType{ in_itemType }
	{
	}
};

// Param:
//  uint64 id : 아이템을 사용하는 주체 id
//  uint64 targetID : 아이템을 적용하는 대상
//  ItemType itemType : 사용하는 아이템의 타입, 실패 시 ItemType::None 반환
//  float Stamina: 아이템 사용 이후의 사용자 스테미나
//  
//  
struct SCUseItem : Header
{
	uint64 id{};
	uint64 targetID{};

	ItemType itemType{ ItemType::None };

	SCUseItem() = default;
	SCUseItem(const uint64 in_id, const uint64 in_targetID, const ItemType in_itemType) :
		Header{ sizeof(SCUseItem), PacketType::SC_USE_ITEM },
		id{ in_id },
		targetID{ in_targetID },
		itemType{ in_itemType }
	{
	}
};


// Param:
//  uint64 targetID : 아이템을 적용하는 대상
//  ItemType itemType : 사용하는 아이템의 타입
struct CSUseItem : Header
{
	uint64 targetID{};
	ItemType itemType{ ItemType::None };
	
	CSUseItem() = default;
	CSUseItem(const uint64 in_targetID, const ItemType in_itemType) :
		Header{ sizeof(CSUseItem), PacketType::CS_USE_ITEM },
		targetID{ in_targetID },
		itemType{ in_itemType }
	{
	}
};


struct SCStatusUpdate : Header
{
	uint64 id{};
	float hp{};
	float stamina{};
	float bullet{};
	float gold{};
	float armor{};
	SCStatusUpdate() = default;
	SCStatusUpdate(const uint64 in_id, const float in_hp, const float in_stamina, const float in_bullet, const float in_gold, const float in_armor) :
		Header{ sizeof(SCStatusUpdate), PacketType::SC_STATUS_UPDATE },
		id{ in_id },
		hp{ in_hp },
		stamina{ in_stamina },
		bullet{ in_bullet },
		gold{ in_gold },
		armor{ in_armor }
	{
	}
};

#pragma pack(pop)
COMMON_END
