#pragma once
#include "Session.h"
#include "Protocol.h"
#include "Object.h"

class Room;

struct PlayerStatus
{
	float hp{};
	float stamina{};
	float attack{};
	float bullet{};
	float gold{};
	float armor{};


public:
	void Init(const Common::PlayerType type)
	{
		// todo: type에 따른 status 초기화
		switch (type)
		{
		case Common::PlayerType::LobbyPlayer: case Common::PlayerType::Ghost:
		{
			hp = 0.f;
			stamina = 0.f;
			attack = 0.f;
			bullet = 0.f;
			gold = 0.f;
			armor = 0.f;
		}
		break;
		case Common::PlayerType::Player:
		{
			hp = Common::PlayerConstants::Hp;
			stamina = Common::PlayerConstants::Stamina;
			attack = Common::PlayerConstants::Attack;
			bullet = Common::PlayerConstants::Bullet;
			gold = Common::PlayerConstants::Gold;
			armor = Common::PlayerConstants::Armor;
		}
		break;
		default:
			break;
		}
	}

};

class Player : public Object
{
	// --
	// method
	// --
public:
	void Init(const std::shared_ptr<Room>& room, const SessionPtr session, const uint32 id);
	void Clear();

	// player type 변경. clear가 true면 type 변경과 함께 Clear()도 실행
	void ChangePlayerType(const Common::PlayerType type, const bool clear = true);


	// 아이템을 사용 가능한지 여부 검사하고, 가능 시 사용 처리까지 하는 메서드.
	// 아이템이 사용 불가능하다면 false를 반환한다.
	bool TryConsumeItem(const Common::ItemType item_type);

	void ApplyItemEffect(const Common::CSUseItem packet, const PlayerPtr target);

	// 플레이어의 체력을 깎는 것은 TakeDamage 메서드를 통해서만 가능하다.
	// 이를 통해 유령 플레이어 생성을 처리한다.
	void TakeDamage(const float damage);
public:	
	


	// --
	// handler method
	// --
public:
	void HandleMove(const Common::CSMovePlayer& packet);
	void HandlePickItem(const Common::CSPickItem& packet);


	// --
	// getter and setter 
	// --
public:
	const Vec3f& GetPosition() const { return _position; }

	const Vec3f& GetDirection() const { return _direction; }

	SessionPtr GetOwnerSession() const { return _ownerSession; }

	uint8 GetAnimationState() const { return _animationState; }

	Common::PlayerType GetType() const { return _type; }

	bool GetReady() const { return _ready; }
	void SetReady(const bool ready) { _ready = ready; }

	PlayerStatus GetStatus() const { return _status; }

	int GetItemCount(const Common::ItemType item_type) const;

	// --
	// player ingame variables
	// --
private:

	// move

	Vec3f _position{};
	Vec3f _direction{};
	uint8 _animationState{};

	// state

	bool _ready{ false };


	// system

	std::array<int, static_cast<size_t>(Common::ItemType::End)> _inventory{}; 
	PlayerStatus _status{};


	// --
	// network
	// --

private:
	Common::PlayerType _type{ Common::PlayerType::None };
	SessionPtr _ownerSession{ nullptr };
	std::weak_ptr<Room> _room{};


};

