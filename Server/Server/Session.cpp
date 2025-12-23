#include "pch.h"
#include "session.h"
#include "OverlappedEx.h"

Session::Session()
{
	ZeroMemory(&OverEx, sizeof(OverEx));
}

Session::Session(SOCKET socket)
{
	ClientSocket = socket;
	OverEx.clientSocket = socket;
	ZeroMemory(&OverEx, sizeof(OverEx));
}

Session::~Session()
{
	if (INVALID_SOCKET != ClientSocket) {
		closesocket(ClientSocket);
	}
}
