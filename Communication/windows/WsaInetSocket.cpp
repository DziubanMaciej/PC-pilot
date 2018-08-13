#include "Communication/windows/WsaInetSocket.h"
#include "Communication/windows/Wsa.h"

#include <winsock2.h>
#include <ws2tcpip.h>

WsaInetSocket::WsaInetSocket(const InetAddress & address, bool reusable) : WsaSocket(AF_INET, SOCK_STREAM, 0), connected(false) {
	::setsockopt(getSocket(), SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&reusable), 1);

	auto addressStruct = Wsa::convertInetAddress(address);
	auto result = ::bind(getSocket(), reinterpret_cast<sockaddr*>(&addressStruct), sizeof(addressStruct));
	if (result == SOCKET_ERROR) Wsa::error("Binding");
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
	if (connectedSocket == INVALID_SOCKET) Wsa::error("Accepting");
	else connected = true;
}

void WsaInetSocket::connect(InetAddress & address) {
	auto addressStruct = Wsa::convertInetAddress(address);
	auto result = ::connect(getSocket(), reinterpret_cast<sockaddr*>(&addressStruct), sizeof(addressStruct));
	if (result == SOCKET_ERROR) Wsa::error("Connecting");
	else connected = true;
}

void WsaInetSocket::send(const std::string & message) {
	if (!isConnected()) throw std::exception("Not connected");
	::send(getSocket(), message.c_str(), message.size(), 0);
}

std::string WsaInetSocket::receive()
{
	return std::string();
}