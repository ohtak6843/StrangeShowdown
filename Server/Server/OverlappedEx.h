#pragma once

class OverlappedEx
{
public:
	// recv 전용 생성자. 
	OverlappedEx();

	// send 전용 생성자
	OverlappedEx(unsigned char* packet);

public:
	// overlapped IO 구조체
	WSAOVERLAPPED	over;
	// 클라이언트 소켓
	SOCKET			clientSocket{ INVALID_SOCKET };
	// 작업 버퍼
	WSABUF			wsabuf{};
	// 데이터 버퍼
	PacketType		dataBuffer{};
	IOOperation		operation{};
};
