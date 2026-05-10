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
	// ingame system
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

	SC_SPAWN_OBJECT,

	SC_MOVE_OBJECT,

	SC_MOVE_PLAYER,
	CS_MOVE_PLAYER,

	SC_TELEPORT_PLAYER,
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
	Header(const uint16 size, const PacketType type) :
		size{ size },
		type{ type }
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
	SCGiveRoomList(const uint16 _roomCount) :
		Header{ sizeof(SCGiveRoomList), PacketType::SC_GIVE_ROOM_LIST },
		roomCount{ _roomCount }
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
	SCCreateRoom(const bool _success) :
		Header{ sizeof(SCCreateRoom), PacketType::SC_CREATE_ROOM },
		success{ _success }
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
	CSCreateRoom(const char* _name, bool _hasPassword, const char* _password = "") :
		Header{ sizeof(CSCreateRoom), PacketType::CS_CREATE_ROOM },
		hasPassword{ _hasPassword }
	{
		strncpy_s(name, _name, sizeof(name) - 1);
		if (_hasPassword && _password != nullptr)
		{
			strncpy_s(password, _password, sizeof(password) - 1);
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
	SCJoinRoom(const bool _success, const uint64 _hostID = 0, const uint64 _myID = 0) :
		Header{ sizeof(SCJoinRoom), PacketType::SC_JOIN_ROOM },
		success{ _success },
		hostID{ _hostID },
		MyID{ _myID }
	{
	}
};

// CSJoinRoom
struct CSJoinRoom : Header
{
	uint32 roomID{};
	char password[21]{};
	CSJoinRoom() = default;
	CSJoinRoom(const uint32 _roomID, const char* _password = "") :
		Header{ sizeof(CSJoinRoom), PacketType::CS_JOIN_ROOM },
		roomID{ _roomID }
	{
		if (_password != nullptr)
		{
			strncpy_s(password, _password, sizeof(password) - 1);
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
	SCReady(const uint64 _id, const bool _ready) :
		Header{ sizeof(SCReady), PacketType::SC_READY },
		id{ _id },
		ready{ _ready }
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
	CSReady(const bool _ready) :
		Header{ sizeof(CSReady), PacketType::CS_READY },
		ready{ _ready }
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
	SCStartGame(const bool _start) :
		Header{ sizeof(SCStartGame), PacketType::SC_START_GAME },
		start{ _start }
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
	SCChat(const uint64 _id, const uint16 _size) :
		Header{ static_cast<uint16>(sizeof(SCChat) + _size), PacketType::SC_CHAT },
		id{ _id }
	{
	}
};

// CSChat
// no param
// Brief:
//  [ 가변 크기 패킷 ]
//  서버로 채팅 메시지를 보내는 패킷. 채팅 메시지를 뒤에 이어붙인다.
struct CSChat : Header
{
	CSChat() = default;
	CSChat(const uint16 _size) :
		Header{ static_cast<uint16>(sizeof(CSChat) + _size), PacketType::CS_CHAT }
	{
	}
};

// Param:
//	Vec3f dir
// 클라이언트 로그인 요청
struct SCSpawnObject : Header
{
	uint64 objectID{};
	Vec3f pos{};
	Vec3f dir{};

	SCSpawnObject() = default;
	SCSpawnObject(const uint64 _objectID, const Vec3f& _pos, const Vec3f& _dir) :
		Header{ sizeof(SCSpawnObject), PacketType::SC_SPAWN_OBJECT },
		objectID{ _objectID },
		pos{ _pos },
		dir{ _dir }
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
	SCMovePlayer(const uint64 _id, const Vec3f& _pos, const Vec3f& _dir, const uint8 _state) :
		Header{ sizeof(SCMovePlayer), PacketType::SC_MOVE_PLAYER },
		id{ _id },
		pos{ _pos },
		dir{ _dir },
		state{ _state }
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
	CSMovePlayer(const Vec3f& _pos, const Vec3f& _dir, const uint8 _state) :
		Header{ sizeof(CSMovePlayer), PacketType::CS_MOVE_PLAYER },
		pos{ _pos },
		dir{ _dir },
		state{ _state }
	{
	}
};


#pragma pack(pop)
COMMON_END
