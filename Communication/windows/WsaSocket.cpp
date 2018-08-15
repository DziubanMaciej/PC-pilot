#include "Communication/windows/WsaSocket.h"
#include "Communication/windows/Wsa.h"
#include "Utils/ApplicationError.h"

#include <winsock2.h>
#include <ws2tcpip.h>

WsaSocket::WsaSocket(int addressFamily, int type, int protocol) : socket(::socket(addressFamily, type, protocol)) {
	if (getSocket() == INVALID_SOCKET) {
		auto errorCode = WSAGetLastError();
		ApplicationError::unrecoverable("Socket constructing", errorCode);
	}
}

WsaSocket::~WsaSocket() {
	closesocket(getSocket());
}

const SOCKET & WsaSocket::getSocket() {
	return socket;
}