#pragma once

class Session;

enum class IOOperation
{
	NONE,
	ACCEPT,
	RECV,
	SEND
};

class OverlappedEx
{
public:
	OverlappedEx();

	// --
	// method
	// --
	void Clear();
	void Reset();

	void PrepareRecv();
	void PrepareSend(const SendBuffer& packet, const int index);
	void PrepareAccept();

	// --
	// getter and setter
	// --
	IOOperation GetOperation() const { return _operation; }
	WSAOVERLAPPED& GetOverlapped() { return _overlapped; }
	WSABUF& GetWsabuf() { return _wsabuf; }
	int GetSendIndex() const { return _sendIndex; }

	void SetSession(const std::shared_ptr<Session>& session) { _session = session; }

private:
	// 추가로 데이터를 관리할 주체(Session 또는 Packet)의 포인터를 들고 있으면 좋음
	WSAOVERLAPPED	_overlapped{};

	// IO 연산 구분
	IOOperation				_operation{};

	// TODO: 나중에 vector화
	WSABUF		_wsabuf{};

	std::weak_ptr<Session>	_session{};
	int _sendIndex{ -1 };

	// for send
	std::vector<char>		_dataBuffer{};
};
