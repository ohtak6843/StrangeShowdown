#pragma once

constexpr int PORT_NUMBER{ 7777 };
constexpr const char* SERVER_IP{ "127.0.0.1" };

constexpr float MOVE_PACKET_TIME_MS{ 75.f }; // 초당 13.3회
constexpr float MAX_NETWORK_DELAY_MS{ 100.f }; // 최대 네트워크 딜레이

#define COMMON_START	namespace Common {
#define COMMON_END		}

COMMON_START
enum class PlayerMeshType : uint8
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

enum class PacketType : unsigned char
{
	NONE = 0,

	// prepare
	SC_LOGIN,
	CS_LOGIN,

	SC_SPAWN_OBJECT,

	SC_MOVE_OBJECT,
	CS_MOVE_PLAYER
};

// todo: 나중에 packet size, type의 크기를 바꿔야 함.
#pragma pack(push, 1)
struct Header
{
	unsigned char	size{ sizeof(Header) };
	PacketType		type{ PacketType::NONE };

	Header() = default;
	Header(const unsigned char size, const PacketType type) :
		size{ size },
		type{ type }
	{}

	// TODO: 직렬화 코드
	//std::vector<char> Serialize() const
	//{
	//	std::vector<char> buffer(size);
	//	std::memcpy(buffer.data(), this, size);
	//	return buffer;
	//}

};


// No Param
// 서버 로그인 승인 패킷
struct SCLogin : Header
{
	SCLogin() :
		Header{ sizeof(SCLogin), PacketType::SC_LOGIN }
	{}
};


// No Param
// 클라이언트 로그인 요청 패킷
struct CSLogin : Header
{
	CSLogin() :
		Header{ sizeof(CSLogin), PacketType::CS_LOGIN }
	{}
};

// Param:
//		Vec3f dir
// 클라이언트 로그인 요청 패킷
struct SCSpawnObject : Header
{
	unsigned long long objectID{};
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
struct SCMoveObject : Header
{
	unsigned long long objectID{};
	Vec3f pos{};
	Vec3f dir{};

	SCMoveObject() = default;
	SCMoveObject(const uint64 _objectID, const Vec3f& _pos, const Vec3f& _dir) :
		Header{ sizeof(SCMoveObject), PacketType::SC_MOVE_OBJECT },
		objectID{ _objectID },
		pos{ _pos },
		dir{ _dir }
	{}
};

// Param:
//		Vec3f dir
// 클라이언트 로그인 요청 패킷
struct CSMovePlayer : Header
{
	Vec3f pos{};
	Vec3f dir{};

	CSMovePlayer() = default;
	CSMovePlayer(const Vec3f& _pos, const Vec3f& _dir) :
		Header{ sizeof(CSMovePlayer), PacketType::CS_MOVE_PLAYER },
		pos{ _pos },
		dir{ _dir }
	{}
};


#pragma pack(pop)
COMMON_END
