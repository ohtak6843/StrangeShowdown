#pragma once
#include "Session.h"
#include "Protocol.h"

class Player
{
public:


	// getter and setter 

	const Vec3f& GetPosition() const
	{
		return _position;
	}

	const Vec3f& GetDirection() const
	{
		return _direction;
	}

	SessionPtr GetOwnerSession() const
	{
		return _ownerSession;
	}
	void SetOwnerSession(const SessionPtr session)
	{
		_ownerSession = session;
	}

	uint8 GetState() const
	{
		return _state;
	}


	void HandleMove(const Common::CSMovePlayer& packet);

	// getter and setter
	bool IsReady() const { return ready; }
	void SetReady(bool _ready) { ready = _ready; }

private:

	// player ingame info
	Vec3f _position{};
	Vec3f _direction{};

	uint8 _state{};

	bool ready{ false };

	// owner
	SessionPtr _ownerSession{ nullptr };

};

