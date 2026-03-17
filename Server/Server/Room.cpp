#include "pch.h"
#include "Room.h"

void Room::PushJob(Job& job)
{
	_jobQueue.push(job);
}

void Room::Init(const uint32 roomID, const Common::CSCreateRoom& packet)
{
	_roomID = roomID;
	_name = packet.name;
	_hasPassword = packet.hasPassword;
	_password = packet.password;
}

void Room::Update()
{
	bool expected{ false };

	// job 처리는 하나의 스레드만 들어가도록 보장
	if (_busy.compare_exchange_strong(expected, true))
	{
		// 모든 큐 비우기
		Job job;
		while (true == _jobQueue.try_pop(job))
		{
			job.Execute();
		}

		_busy = false;
	}
}