#pragma once
#include "Session.h"
#include "Protocol.h"

enum class PlayerState : uint32
{
	NONE = 0,
	LOBBY = 1,
	INGAME = 2,
	GHOST = 3,
	GHOST_SHERIFF = 4,
};

struct PlayerStatus
{
	float hp{};
	float stamina{};
	float attack{};
	float bullet{};
	float gold{};

public:
	void Init(const PlayerState state)
	{
		// todo: state에 따른 status 초기화

		hp = Common::PlayerConstants::Hp;
		stamina = Common::PlayerConstants::Stamina;
		attack = Common::PlayerConstants::Attack;
		bullet = Common::PlayerConstants::Bullet;
		gold = Common::PlayerConstants::Gold;
	}

};

class Player
{

	// --
	// method
	// --
public:
	void Init(const PlayerState state);


	// 아이템을 사용 가능한지 여부 검사하고, 가능 시 사용 처리까지 하는 메서드.
	// 아이템이 사용 불가능하다면 false를 반환한다.
	bool TryConsumeItem(const Common::ItemType item_type);


public:	
	


	// --
	// handler method
	// --
public:
	void HandleMove(const Common::CSMovePlayer& packet);
	


	// --
	// getter and setter 
	// --
public:
	const Vec3f& GetPosition() const { return _position; }

	const Vec3f& GetDirection() const { return _direction; }

	SessionPtr GetOwnerSession() const { return _ownerSession; }
	void SetOwnerSession(const SessionPtr session) { _ownerSession = session; }

	uint8 GetAnimationState() const { return _animationState; }

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
	PlayerState _state{ PlayerState::NONE };
	// owner
	SessionPtr _ownerSession{ nullptr };


};

