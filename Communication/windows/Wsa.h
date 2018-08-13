#pragma once

#include "Communication/SocketContext.h"

#include <memory>
#include <string>

struct sockaddr_in;

class Wsa : public SocketContext {
public:
	Wsa();
	~Wsa();
	
	std::unique_ptr<InetAddress> getInetAddressAny(short port) override;
	std::unique_ptr<InetAddress> getInetAddressLoopback(short port) override;
	std::unique_ptr<InetAddress> getInetAddress(uint32_t address, short port) override;
	std::unique_ptr<ConnectionOrientedSocket> getInetSocket(const InetAddress &address, bool reusable) override;

	static void error(const std::string &message);
	static sockaddr_in convertInetAddress(const InetAddress &address);
};
