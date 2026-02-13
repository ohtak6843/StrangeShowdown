#include "pch.h"
#include "OverlappedEx.h"
#include "Session.h"
#include "protocol.h"

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
		return;
	}
	_operation = IOOperation::RECV;
}

void OverlappedEx::PrepareSend(const std::vector<char>& packet)
{
	Clear();
	_dataBuffer.assign_range(packet);
	Common::Header& header{ *reinterpret_cast<Common::Header*>(_dataBuffer.data()) };
	_wsabuf.len = header.GetSize();
	_wsabuf.buf = _dataBuffer.data();
	_operation = IOOperation::SEND;
}

void OverlappedEx::PrepareAccept()
{
	Clear();
	_operation = IOOperation::ACCEPT;
}
