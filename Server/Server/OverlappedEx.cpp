#include "pch.h"
#include "OverlappedEx.h"

OverlappedEx::OverlappedEx() :
	operation{ IOOperation::RECV }
{
	wsabuf.len = BUFFER_SIZE;
	wsabuf.buf = dataBuffer.data();
	ZeroMemory(&over, sizeof(over));
}

OverlappedEx::OverlappedEx(unsigned char* packet)
{
	wsabuf.len = packet[0];
	wsabuf.buf = dataBuffer.data();
	ZeroMemory(&over, sizeof(over));
	operation = IOOperation::SEND;
	memcpy(dataBuffer.data(), packet, packet[0]);

}