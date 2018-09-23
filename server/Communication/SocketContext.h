#pragma once

#include "Utils/InstanceLimited.h"
#include "Communication/ConnectionlessSocket.h"

#include <memory>
#include <vector>
#include <string>

class SocketContext : public OneInstanced<SocketContext> {
protected:
	SocketContext() = default;
	SocketContext(const SocketContext&) = delete;
public:
	static std::unique_ptr<SocketContext> create();
	virtual ~SocketContext() = default;

	virtual std::unique_ptr<ConnectionlessSocket> getInetSocket(const InetAddress &address, bool reusable, bool broadcast) = 0;
	virtual std::vector<std::string> getInetAddresses(bool ipv4, bool ipv6, bool loopback) = 0;
};
