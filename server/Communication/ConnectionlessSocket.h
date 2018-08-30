#pragma once

#include <string>
#include <vector>

struct InetAddress;

class ConnectionlessSocket {
public:
	ConnectionlessSocket() {}
	virtual ~ConnectionlessSocket() {}

	virtual void bind(const InetAddress &address, bool reusable) = 0;
	virtual void send(const std::string &message, const InetAddress &address) = 0;
	virtual bool receive(std::vector<unsigned char> &result, size_t bufferSize, uint32_t timeoutMs) = 0;
};
