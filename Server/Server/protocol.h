#pragma once

constexpr int PORT_NUMBER{ 7777 };
constexpr const char* SERVER_IP{ "127.0.0.1" };

constexpr float MOVE_PACKET_TIME_MS{ 75.f }; // 초당 13.3회
constexpr float MAX_NETWORK_DELAY_MS{ 100.f }; // 최대 네트워크 딜레이

#define PACKET_START	namespace packet {
#define PACKET_END		}

PACKET_START
enum class Type : unsigned char
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
	Type			type{ Type::NONE };

	Header() = default;
	Header(const unsigned char size, const Type type) :
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
		Header{ sizeof(SCLogin), Type::SC_LOGIN }
	{}
};


// No Param
// 클라이언트 로그인 요청 패킷
struct CSLogin : Header
{
	CSLogin() :
		Header{ sizeof(CSLogin), Type::CS_LOGIN }
	{}
};

// Param:
//		Vec3f dir
// 클라이언트 로그인 요청 패킷
struct SCSpawnObject : Header
{
	unsigned long long objectID;
	Vec3f pos;
	Vec3f dir;

	SCSpawnObject(const uint64 _objectID, const Vec3f& _pos, const Vec3f& _dir) :
		Header{ sizeof(SCSpawnObject), Type::SC_SPAWN_OBJECT },
		objectID{ _objectID },
		pos{ _pos },
		dir{ _dir }
	{
	}
};


// Param:
//		uint64 id
//		Vec3f pos
//		Vec3f dir
struct SCMoveObject : Header
{
	unsigned long long objectID;
	Vec3f pos;
	Vec3f dir;

	SCMoveObject(const uint64 _objectID, const Vec3f& _pos, const Vec3f& _dir) :
		Header{ sizeof(SCMoveObject), Type::SC_MOVE_OBJECT },
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
	Vec3f pos;
	Vec3f dir;

	CSMovePlayer(const Vec3f& _pos, const Vec3f& _dir) :
		Header{ sizeof(CSMovePlayer), Type::CS_MOVE_PLAYER },
		pos{ _pos },
		dir{ _dir }
	{}
};


#pragma pack(pop)
PACKET_END
