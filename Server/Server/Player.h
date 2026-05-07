#pragma once
#include "Session.h"
#include "Protocol.h"

class Player
{
	// --
	// handler method
	// --
public:
	void HandleMove(const Common::CSMovePlayer& packet);
	
	// --
	// getter and setter 
	// --
public:
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



	bool GetReady() const { return _ready; }
	void SetReady(const bool ready) { _ready = ready; }



	// --
	// player ingame variables
	// --
private:
	Vec3f _position{};
	Vec3f _direction{};
	uint8 _state{};
	bool _ready{ false };

	// owner
	SessionPtr _ownerSession{ nullptr };

};

