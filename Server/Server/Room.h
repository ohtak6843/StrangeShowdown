#pragma once

#include "Job.h"

class Room
{
public:

	// 밀려있는 job을 실행.
	void Update();

	// jobQueue에 LF로 job을 push
	void PushJob(Job& job);

private:
	// atomic 변수 필요 (job Queue 한 스레드 보장
	std::atomic<bool> _busy{ false };
	concurrency::concurrent_queue<Job> _jobQueue;

	// 현재 방에 있는 플레이어 수
	std::unordered_map<uint64, std::shared_ptr<Player>> _players{};

};

