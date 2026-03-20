#pragma once
#include <cstdint>

constexpr int PORT_NUMBER{ 7777 };
constexpr const char* SERVER_IP{ "127.0.0.1" };

constexpr float MOVE_PACKET_TIME_MS{ 75.f }; // 초당 13.3회
constexpr float MAX_NETWORK_DELAY_MS{ 100.f }; // 최대 네트워크 딜레이

#define COMMON_START	namespace Common {
#define COMMON_END		}

COMMON_START
enum class PlayerType : uint8
{
	Badguy,
	BuisinessMan,
	Cowboy,
	Cowgirl,
	Gunman,
	Sheriff,
	Woman,
	WorkingGirl,
};

enum class PlayerState : uint8
{
	None		= 0,
	Idle		= 1 << 0,
	Jumping		= 1 << 1,
	HoldItem	= 1 << 2,
	ArmedPistol = 1 << 3,
	ArmedHammer = 1 << 4,
	Aiming		= 1 << 5,
	LookingUp	= 1 << 6,
	Dead		= 1 << 7,
};

enum class PacketType : uint16
{
	NONE = 0,

	// prepare
	SC_LOGIN,
	CS_LOGIN,

	// room
	SC_GIVE_ROOM_LIST,
	CS_GET_ROOM_LIST,

	SC_CREATE_ROOM,
	CS_CREATE_ROOM,

	SC_JOIN_ROOM,
	CS_JOIN_ROOM,

	// object
	SC_SPAWN_OBJECT,

	SC_MOVE_OBJECT,

	SC_MOVE_PLAYER,
	CS_MOVE_PLAYER
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
	{}
};


// 서버 로그인 승인 패킷
struct SCLogin : Header
{
	SCLogin() :
		Header{ sizeof(SCLogin), PacketType::SC_LOGIN }
	{}
};


// No Param
// 클라이언트 로그인 요청
struct CSLogin : Header
{
	CSLogin() :
		Header{ sizeof(CSLogin), PacketType::CS_LOGIN }
	{}
};

// [ 가변 크기 패킷 ]
// Param:
//		uint16 roomCount
// 서버 방 리스트 제공
// 방의 갯수만큼 크기를 조절해 RoomInfo를 이어 보낸다.
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
// 클라이언트 방 리스트 제공 요청
struct CSGetRoomList : Header
{
	CSGetRoomList() :
		Header{ sizeof(CSGetRoomList), PacketType::CS_GET_ROOM_LIST }
	{
	}
};


// Param:
//	bool success
// 서버 방 생성 여부
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
// 클라이언트 방 생성 요청
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
// 
// 서버 방 입장 승인
struct SCJoinRoom : Header
{
	bool success{ false };
	SCJoinRoom() = default;
	SCJoinRoom(const bool _success) :
		Header{ sizeof(SCJoinRoom), PacketType::SC_JOIN_ROOM },
		success{ _success }
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



// Param:
//		Vec3f dir
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
	{}
};


// Param:
//		uint64 id
//		Vec3f pos
//		Vec3f dir
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
	{}
};

// Param:
//		Vec3f pos
//		Vec3f dir
//		uint8 state
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
	{}
};


#pragma pack(pop)
COMMON_END
