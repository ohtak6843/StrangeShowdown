#include "pch.h"
#include "OverlappedEx.h"
#include "Session.h"

OverlappedEx::OverlappedEx()
{
	Clear();
}

void OverlappedEx::Clear()
{
	ZeroMemory(&_overlapped, sizeof(_overlapped));
	_wsabuf.buf = nullptr;
	_wsabuf.len = 0ul;
	_operation = IOOperation::NONE;
}

void OverlappedEx::PrepareRecv()
{
	Clear();
	if (auto session{ _session.lock() })
	{
		_wsabuf.len = BUFFER_SIZE - session->GetCurrentDataSize();
		_wsabuf.buf = session->GetRecvBuffer().data() + session->GetCurrentDataSize();
	}
	else
	{
		// todo:
		// 세션이 없으면 오류.

	}
	_operation = IOOperation::RECV;
}

void OverlappedEx::PrepareSend(const std::vector<char>& packet)
{
	Clear();
	_dataBuffer.assign_range(packet);
	_wsabuf.len = static_cast<uint8>(_dataBuffer[0]);
	_wsabuf.buf = _dataBuffer.data();
	_operation = IOOperation::SEND;
}

void OverlappedEx::PrepareAccept()
{
	Clear();
	_operation = IOOperation::ACCEPT;
}
