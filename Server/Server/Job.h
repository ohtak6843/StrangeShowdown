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

// todo: delayed job
//
//struct DelayedJob {
//    TimePoint executeTime;
//    JobAction action;
//
//    // 시간이 가장 '빠른' 작업이 Top으로 오게끔 부등호 방향 설정 (Min Heap)
//    bool operator<(const DelayedJob& other) const {
//        return executeTime > other.executeTime;
//    }
//};


//class JobQueue {
//private:
//    std::queue<JobAction> readyJobs;
//    std::priority_queue<DelayedJob> delayedJobs;
//
//public:
//    // 지연된 작업 예약
//    void PushDelayed(int delayMilliseconds, JobAction action) {
//        auto targetTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(delayMilliseconds);
//        delayedJobs.push({ targetTime, std::move(action) });
//    }
//
//    // Job Thread의 메인 루프 (Tick / Update)
//    void Flush() {
//        auto now = std::chrono::steady_clock::now();
//
//        // 1. 시간이 다 된 지연된 작업을 일반 작업 큐로 이동
//        while (!delayedJobs.empty() && delayedJobs.top().executeTime <= now) {
//            readyJobs.push(std::move(delayedJobs.top().action));
//            delayedJobs.pop();
//        }
//
//        // 2. 준비된 일반 작업(Ready Jobs) 모두 실행
//        while (!readyJobs.empty()) {
//            auto action = std::move(readyJobs.front());
//            readyJobs.pop();
//            action();
//        }
//    }
//};