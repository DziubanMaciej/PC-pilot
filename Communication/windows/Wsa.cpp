#include "Communication/windows/Wsa.h"
#include "Communication/InetAddress.h"
#include "Communication/windows/UdpSocket.h"
#include "Utils/ApplicationError.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
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

std::unique_ptr<InetAddress> Wsa::getInetAddress(const std::string & address, short port) {
	uint32_t ip;
	auto result = InetPton(AF_INET, address.c_str(), &ip);
	if (result != 1) ApplicationError::exception("InetAddress parse");
	return getInetAddress(ip, port);
}

std::unique_ptr<ConnectionlessSocket> Wsa::getInetSocket(const InetAddress &address, bool reusable) {
	return std::make_unique<UdpSocket>(address, reusable);
}

std::unique_ptr<Wsa::Adapter[]> Wsa::getAdapters() {
	std::unique_ptr<Adapter[]> adapters;
	ULONG count = 2;
	ULONG result;
	do {
		adapters = std::make_unique<Adapter[]>(count);
		ULONG size = count * sizeof(Adapter);
		result = GetAdaptersAddresses(AF_UNSPEC, 0, 0, adapters.get(), &size);
		count *= 2;
	} while (result == ERROR_BUFFER_OVERFLOW);
	return adapters;
}

std::string Wsa::ipToString(int addressFamily, void *address) {
	char buffer[64] = {};
	auto result = InetNtop(addressFamily, address, buffer, sizeof(buffer));
	if (result == nullptr) {
		ApplicationError::unrecoverable("Address parse");
	}
	return std::string{ buffer };
}

std::vector<std::string> Wsa::getInetAddresses(bool ipv4, bool ipv6, bool loopback) {
	auto adapters = getAdapters();

	std::vector<std::string> result;
	for (auto adapter = adapters.get(); adapter != nullptr; adapter = adapter->Next) {
		for (auto unicastAddress = adapter->FirstUnicastAddress; unicastAddress != nullptr; unicastAddress = unicastAddress->Next) {
			sockaddr *address = unicastAddress->Address.lpSockaddr;

			auto family = address->sa_family;
			if (family == AF_INET) {
				if (!ipv4) continue;

				auto a = &((sockaddr_in*)address)->sin_addr;
				std::string str = ipToString(family, a);

				if (!loopback && str.find("127") == 0) continue;
				result.push_back(str);
			}
			else if (family == AF_INET6) {
				if (!ipv6) continue;
				auto a = &((sockaddr_in6*)address)->sin6_addr;
				std::string str = ipToString(family, a);

				if (!loopback && str.find("::1") == 0) continue;
				result.push_back(str);
			}
			else {
				ApplicationError::unrecoverable("Unknown address family");
			}
		}
	}
	return result;
}

sockaddr_in Wsa::convertInetAddress(const InetAddress & address) {
	sockaddr_in result;
	std::memset(&result, 0, sizeof(sockaddr_in));
	result.sin_family = AF_INET;
	result.sin_addr.s_addr = address.address;
	result.sin_port = htons(address.port);
	return result;
}
