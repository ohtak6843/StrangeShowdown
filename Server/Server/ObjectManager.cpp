#include "pch.h"
#include "ObjectManager.h"
#include "Session.h"
#include "OverlappedEx.h"
#include "Room.h"
#include "Player.h"

void ObjectManager::Init()
{
	for(auto& [type, count] : _poolConfig)
	{
		auto type_ull{ static_cast<size_t>(type)};
		if (type_ull >= _objectPool.size() || type_ull < 0)
		{
			continue;
		}
		auto& pool{ _objectPool[type_ull] };
		for (size_t i = 0; i < count; ++i)
		{
			switch (type)
			{
			case ObjectType::Session:
			{
				pool.push(std::make_shared<Session>());
				break;
			}
			case ObjectType::Player:
			{
				pool.push(std::make_shared<Player>());
				break;
			}
			case ObjectType::Room:
			{
				pool.push(std::make_shared<Room>());
				break;
			}
			default:
				break;
			}
		}
	}

	std::println("ObjectManager Init Successed.");
}