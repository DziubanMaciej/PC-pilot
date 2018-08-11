#pragma once

#include "Communication/ConnectionOrientedSocket.h"

#include <WinSock2.h>

class WsaSocket : public ConnectionOrientedSocket {
public:
	WsaSocket(int addressFamily, int type, int protocol);
	~WsaSocket() override;
	const SOCKET& getSocket();

private:
	const SOCKET socket;
};
