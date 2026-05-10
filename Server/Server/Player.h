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



	// --
	// player ingame variables
	// --
private:
	Vec3f _position{};
	Vec3f _direction{};
	uint8 _animationState{};
	bool _ready{ false };


	// --
	// player 
	// --

	PlayerState _state{ PlayerState::NONE };
	PlayerStatus _status{};

	// owner
	SessionPtr _ownerSession{ nullptr };


};

