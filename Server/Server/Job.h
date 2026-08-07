#pragma once

class Job
{
public:
	using JobFunc = std::function<void()>;

	Job();
	Job(JobFunc&& func);

	void Execute();

private:
	JobFunc _func;
};

struct DelayedJob
{
	std::chrono::steady_clock::time_point executeTime;
	Job job;

	// 우선순위 큐를 Min-Heap으로 동작하도록 설정 (시간이 작은 것이 먼저 나오게 함)
	bool operator<(const DelayedJob& other) const
	{
		return executeTime > other.executeTime;
	}
};