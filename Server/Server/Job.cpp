#include "pch.h"
#include "Job.h"

Job::Job()
{
}

Job::Job(JobFunc&& func) :
	_func{std::move(func)}
{
}

void Job::Execute()
{
	if (_func)
	{
		_func();
	}
}
