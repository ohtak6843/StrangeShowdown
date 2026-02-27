#pragma once

// type 가능
// owner, Createtime 변수, Cancel() 메소드 사용가능.
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