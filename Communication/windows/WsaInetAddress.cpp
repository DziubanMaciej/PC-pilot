#include "Communication/windows/WsaInetAddress.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdint.h>
#include <string>

WsaInetAddress::WsaInetAddress(const std::string & ip, short port) : WsaInetAddress(parseIp(ip), port) {}

WsaInetAddress::WsaInetAddress(short port) : WsaInetAddress(INADDR_ANY, port) {}

WsaInetAddress::WsaInetAddress(uint32_t ip, short port) : address(std::make_unique<sockaddr_in>()) {
	std::memset(address.get(), 0, sizeof(sockaddr_in));
	address->sin_family = AF_INET;
	address->sin_addr.s_addr = ip;
	address->sin_port = htons(port);
}

sockaddr & WsaInetAddress::sockAddr() {
	return *reinterpret_cast<sockaddr*>(address.get());
}

sockaddr_in & WsaInetAddress::sockAddrIn() {
	return *address;
}

uint32_t WsaInetAddress::parseIp(const std::string & str) {
	uint32_t ip;
	auto result = InetPton(AF_INET, str.c_str(), &ip);
	if (result != 1) throw std::exception("parseIp");
	return ip;
}
