#include "UdpSocket.h"

#include "Communication/windows/Wsa.h"
#include "Utils/ApplicationError.h"

#include <winsock2.h>
#include <ws2tcpip.h>

UdpSocket::UdpSocket(const InetAddress &address, bool reusable) : socket(::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) {
	bind(address, reusable);
}

UdpSocket::~UdpSocket() {
	closesocket(this->socket);
}

void UdpSocket::bind(const InetAddress & address, bool reusable) {
	::setsockopt(this->socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&reusable), sizeof(char));

	auto addressStruct = Wsa::convertInetAddress(address);
	auto result = ::bind(this->socket, reinterpret_cast<sockaddr*>(&addressStruct), sizeof(addressStruct));
	if (result == SOCKET_ERROR) {
		auto errorCode = WSAGetLastError();
		ApplicationError::exception("Socket binding", errorCode);
	}
}

void UdpSocket::send(const std::string & message, const InetAddress &address) {
	sockaddr_in destination = Wsa::convertInetAddress(address);
	auto result = ::sendto(this->socket, message.c_str, message.size(), 0, reinterpret_cast<sockaddr*>(&destination), sizeof(destination));
	if (result == SOCKET_ERROR) {
		auto errorCode = WSAGetLastError();
		ApplicationError::exception("Sending", errorCode);
	}
}

std::string UdpSocket::receive(size_t bufferSize) {
	auto buffer = std::make_unique<char[]>(bufferSize);

	auto result = ::recvfrom(this->socket, buffer.get(), bufferSize, 0, nullptr, 0);
	if (result == SOCKET_ERROR) {
		auto errorCode = WSAGetLastError();
		ApplicationError::exception("Receiving", errorCode);
	}
	return std::string(buffer.get());
}
