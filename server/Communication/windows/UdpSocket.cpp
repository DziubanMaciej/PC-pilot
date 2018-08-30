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
	auto result = ::sendto(this->socket, message.c_str(), message.size(), 0, reinterpret_cast<sockaddr*>(&destination), sizeof(destination));
	if (result == SOCKET_ERROR) {
		auto errorCode = WSAGetLastError();
		ApplicationError::exception("Sending", errorCode);
	}
}

bool UdpSocket::receive(std::vector<unsigned char> &result, size_t bufferSize, uint32_t timeoutMs) {
	result.clear();
	result.resize(bufferSize);

	::setsockopt(this->socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeoutMs), sizeof(timeoutMs));
	auto retVal = ::recvfrom(this->socket, reinterpret_cast<char*>(result.data()), bufferSize, 0, nullptr, 0);
	if (retVal == SOCKET_ERROR) {
		auto errorCode = WSAGetLastError();
		if (errorCode == WSAETIMEDOUT) {
			return false;
		}
		ApplicationError::exception("Receiving", errorCode);
	}
	result.resize(retVal);
	return true;
}
