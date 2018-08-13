#include "Communication/windows/Wsa.h"
#include "Communication/windows/WsaInetSocket.h"
#include "Communication/InetAddress.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

Wsa::Wsa() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData); //TODO check
}

Wsa::~Wsa() {
	WSACleanup();
}

std::unique_ptr<InetAddress> Wsa::getInetAddressAny(short port) {
	return getInetAddress(INADDR_ANY, port);
}

std::unique_ptr<InetAddress> Wsa::getInetAddressLoopback(short port) {
	return getInetAddress(INADDR_LOOPBACK, port);
}

std::unique_ptr<InetAddress> Wsa::getInetAddress(uint32_t address, short port) {
	return std::make_unique<InetAddress>(address, port);
}

std::unique_ptr<ConnectionOrientedSocket> Wsa::getInetSocket(const InetAddress &address, bool reusable) {
	return std::make_unique<WsaInetSocket>(address, reusable);
}

void Wsa::error(const std::string & message) {
	std::cout << message << ": " << WSAGetLastError() << std::endl;
	throw std::exception(message.c_str());
}

sockaddr_in Wsa::convertInetAddress(const InetAddress & address) {
	sockaddr_in result;
	std::memset(&result, 0, sizeof(sockaddr_in));
	result.sin_family = AF_INET;
	result.sin_addr.s_addr = address.getAddress();
	result.sin_port = htons(address.getPort());
	return result;
}
