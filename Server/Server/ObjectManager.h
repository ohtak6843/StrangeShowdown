#pragma once
#include "OverlappedEx.h"

/*
* 필요한 객체 및 메모리를 관리하는 매니저 클래스.
**/

enum class ObjectType : uint8
{
	Session,
	Player,
	Room,
	END,
};

class Session;
class Player;
class Room;

template<typename T> struct ObjectTypeTraits;
template<> struct ObjectTypeTraits<Session> { static constexpr ObjectType Type{ ObjectType::Session }; };
template<> struct ObjectTypeTraits<Player> { static constexpr ObjectType Type{ ObjectType::Player }; };
template<> struct ObjectTypeTraits<Room> { static constexpr ObjectType Type{ ObjectType::Room }; };

class ObjectManager
{
	DECLARE_SINGLE(ObjectManager)


public:
	using ObjectPool = std::queue<std::shared_ptr<void>>;

	void Init();

	// pop 이후 Init이 필요한 객체는 init 호출
	template<typename T>
	std::shared_ptr<T> Pop()
	{
		auto& pool{ _objectPool[static_cast<size_t>(ObjectTypeTraits<T>::Type)] };
		std::shared_ptr<T> obj;
		if (true == pool.empty())
		{
			obj = std::make_shared<T>();
		}
		else
		{
			obj = std::static_pointer_cast<T>(pool.front());
			pool.pop();
		}
		return obj;
	}

	// release 하기 전 반드시 Reset 호출
	template<typename T>
	void Release(std::shared_ptr<T> obj)
	{
		auto& pool{ _objectPool[static_cast<size_t>(ObjectTypeTraits<T>::Type)] };
		pool.push(obj);
	}


private:

	// 객체 별 미리 생성할 개수.
	const std::vector<std::pair<ObjectType, size_t>> _poolConfig{
		{ ObjectType::Session, 1000 },
		{ ObjectType::Player, 1000 },
		{ ObjectType::Room, 100 }
	};

	std::array<ObjectPool, static_cast<size_t>(ObjectType::END)> _objectPool{};

};
