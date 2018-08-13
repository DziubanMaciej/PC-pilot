#include "Communication/windows/WsaInetSocket.h"
#include "Communication/windows/Wsa.h"
#include "Utils/ApplicationError.h"

#include <winsock2.h>
#include <ws2tcpip.h>

WsaInetSocket::WsaInetSocket(const InetAddress & address, bool reusable) : WsaSocket(AF_INET, SOCK_STREAM, 0), connected(false) {
	::setsockopt(getSocket(), SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&reusable), 1);

	auto addressStruct = Wsa::convertInetAddress(address);
	auto result = ::bind(getSocket(), reinterpret_cast<sockaddr*>(&addressStruct), sizeof(addressStruct));
	if (result == SOCKET_ERROR) {
		auto errorCode = WSAGetLastError();
		ApplicationError::unrecoverable("Socket binding", errorCode);
	}
}

bool WsaInetSocket::isConnected() const {
	return connected;
}

void WsaInetSocket::listen() {
	::listen(getSocket(), SOMAXCONN);
}

void WsaInetSocket::accept() {
	int size = sizeof(connectedAddress);
	connectedSocket = ::accept(getSocket(), reinterpret_cast<sockaddr*>(&connectedAddress), &size);
	if (connectedSocket == INVALID_SOCKET) {
		auto errorCode = WSAGetLastError();
		ApplicationError::unrecoverable("Socket accepting", errorCode);
	}
	else connected = true;
}

void WsaInetSocket::connect(InetAddress & address) {
	auto addressStruct = Wsa::convertInetAddress(address);
	auto result = ::connect(getSocket(), reinterpret_cast<sockaddr*>(&addressStruct), sizeof(addressStruct));
	if (result == SOCKET_ERROR) {
		auto errorCode = WSAGetLastError();
		ApplicationError::unrecoverable("Socket connecting", errorCode);
	}
	else connected = true;
}

void WsaInetSocket::send(const std::string & message) {
	if (!isConnected()) ApplicationError::exception("Socket not connected");

	auto result = ::send(getSocket(), message.c_str(), message.size(), 0);
	if (result == SOCKET_ERROR) {
		auto errorCode = WSAGetLastError();
		ApplicationError::unrecoverable("Socket receiving", errorCode);
	}
}

std::string WsaInetSocket::receive(size_t bufferSize) {
	if (!isConnected()) ApplicationError::exception("Socket not connected");

	auto buffer = std::make_unique<char[]>(bufferSize);
	auto result = ::recv(this->connectedSocket, buffer.get(), bufferSize, 0);
	if (result == SOCKET_ERROR) {
		auto errorCode = WSAGetLastError();
		ApplicationError::unrecoverable("Socket receiving", errorCode);
	}
	return std::string(buffer.get());
}
