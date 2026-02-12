#pragma once
#include "Session.h"

class Player
{
public:


	// getter and setter 

	const Vec3f& GetPosition() const
	{
		return _position;
	}
	void SetPosition(const Vec3f& pos)
	{
		_position = pos;
	}

	const Vec3f& GetDirection() const
	{
		return _direction;
	}
	void SetDirection(const Vec3f& dir)
	{
		_direction = dir;
	}

	SessionPtr GetOwnerSession() const
	{
		return _ownerSession;
	}
	void SetOwnerSession(const SessionPtr session)
	{
		_ownerSession = session;
	}

private:
	Vec3f _position{};
	Vec3f _direction{};

	// owner
	SessionPtr _ownerSession{ nullptr };

};

