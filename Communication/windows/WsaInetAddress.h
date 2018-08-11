#pragma once

#include <string>
#include <memory>

struct sockaddr;
struct sockaddr_in;

class WsaInetAddress {
public:
	WsaInetAddress(const std::string &ip, short port);
	WsaInetAddress(short port);
	WsaInetAddress(uint32_t ip, short port);

	sockaddr& sockAddr();
	sockaddr_in& sockAddrIn();

	static uint32_t parseIp(const std::string &str);

private:
	std::unique_ptr<sockaddr_in> address;
};
