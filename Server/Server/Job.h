#pragma once

//// Job의 타입을 구분해야 할 경우를 대비한 열거형
//enum class JobType {
//    PACKET,
//    TICK,
//    SYSTEM
//};
//
//class Job {
//public:
//    // 일반적인 함수나 람다를 담을 수 있는 래퍼
//    using JobHandler = std::function<void()>;
//
//    // 생성자: 실행할 함수를 인자로 받음
//    Job(JobHandler&& handler, JobType type = JobType::PACKET)
//        : _handler(std::move(handler)), _type(type) {
//    }
//
//    // 실제 WorkerThread가 호출할 실행 메서드
//    void Execute() {
//        if (_handler) {
//            _handler();
//        }
//    }
//
//private:
//    JobHandler _handler; // 실행될 로직 (람다 캡처 등을 통해 데이터 포함)
//    JobType    _type;    // 로깅이나 디버깅을 위한 타입 구분
//
//    // owner, Createtime 변수, Cancel() 메소드 사용가능.
//};