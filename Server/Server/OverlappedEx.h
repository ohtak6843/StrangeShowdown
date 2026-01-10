#pragma once

class Session;

enum class IOOperation
{
	NONE,
	ACCEPT,
	RECV,
	SEND
};

class alignas(64) OverlappedEx
{
public:
	OverlappedEx();

	// method
	void Clear();

	void PrepareRecv();
	void PrepareSend(std::vector<char>&& packet);
	void PrepareAccept();


	// getter and setter
	IOOperation GetOperation() const { return _operation; }
	WSAOVERLAPPED& GetOverlapped() { return _overlapped; }
	WSABUF& GetWsabuf() { return _wsabuf; }

	void SetSession(const std::shared_ptr<Session>& session) { _session = session; }

private:
	// wsaoverlapped
	WSAOVERLAPPED			_overlapped{};

	// 추가로 데이터를 관리할 주체(Session 또는 Packet)의 포인터를 들고 있으면 좋음
	std::weak_ptr<Session>	_session{};

	// IO 연산 구분
	IOOperation				_operation{};

	// TODO: 나중에 vector화
	WSABUF		_wsabuf{};

	// for send
	std::vector<char>		_dataBuffer{};
};
