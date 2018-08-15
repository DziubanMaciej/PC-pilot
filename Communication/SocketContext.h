#pragma once

#include "Utils/InstanceLimited.h"
#include "Communication/ConnectionOrientedSocket.h"

#include <memory>

class SocketContext : public OneInstanced<SocketContext> {
public:
	SocketContext() {}
	SocketContext(const SocketContext&) = delete;
	virtual ~SocketContext() {}

	virtual std::unique_ptr<InetAddress> getInetAddressAny(short port) = 0;
	virtual std::unique_ptr<InetAddress> getInetAddressLoopback(short port) = 0;
	virtual std::unique_ptr<InetAddress> getInetAddress(uint32_t address, short port) = 0;
	virtual std::unique_ptr<InetAddress> getInetAddress(const std::string &address, short port) = 0;
	virtual std::unique_ptr<ConnectionOrientedSocket> getInetSocket(const InetAddress &address, bool reusable) = 0;
};
