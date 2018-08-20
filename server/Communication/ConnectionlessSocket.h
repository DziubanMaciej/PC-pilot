#pragma once

#include <string>

struct InetAddress;

class ConnectionlessSocket {
public:
	ConnectionlessSocket() {}
	virtual ~ConnectionlessSocket() {}

	virtual void bind(const InetAddress &address, bool reusable) = 0;
	virtual void send(const std::string &message, const InetAddress &address) = 0;
	virtual std::string receive(size_t bufferSize) = 0;
};
