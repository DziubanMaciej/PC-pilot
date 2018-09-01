#pragma once

#include "Utils/InstanceLimited.h"
#include "Communication/ConnectionlessSocket.h"

#include <memory>
#include <vector>
#include <string>

class SocketContext : public OneInstanced<SocketContext> {
public:
	SocketContext() {}
	SocketContext(const SocketContext&) = delete;
	virtual ~SocketContext() {}

	virtual std::unique_ptr<ConnectionlessSocket> getInetSocket(const InetAddress &address, bool reusable) = 0;
	virtual std::vector<std::string> getInetAddresses(bool ipv4, bool ipv6, bool loopback) = 0;
};
