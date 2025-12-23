#pragma once

#include "OverlappedEx.h"

class Session
{
public:
	Session();
	Session(SOCKET socket);
	~Session();



public:
	// RECV에 사용할 Overlapped 변수
	OverlappedEx	OverEx{};
	SOCKET			ClientSocket{ INVALID_SOCKET };

	// 패킷 재조립을 위한 패킷 처리 후 남은 데이터 크기를 저장 
	int				CurrentDataSize{ 0 };
	IOState			IOState{ IOState::NONE };

};