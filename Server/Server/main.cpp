#include "pch.h"
#include "IOCP.h"
#include "PacketHandler.h"
#include "RoomManager.h"
#include "ObjectManager.h"


int main()
{
	// GET_SINGLE(Game)->Init();
	PacketHandler::Init();
	GET_SINGLE(ObjectManager)->Init();
	auto ret{ GET_SINGLE(IOCP)->Init() };
	if (false == ret)
	{
		return -1;
	}

	GET_SINGLE(IOCP)->Start();
	
	while (true)
	{
		std::this_thread::yield();
	}
}