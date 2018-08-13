#pragma once

#include <string>

class InetAddress;

class ConnectionOrientedSocket {
public:
	ConnectionOrientedSocket() {}
	virtual ~ConnectionOrientedSocket() {}

	virtual bool isConnected() const = 0;

	virtual void listen() = 0;
	virtual void accept() = 0;
	virtual void connect(InetAddress &address) = 0;

	virtual void send(const std::string &message) = 0;
	virtual std::string receive(size_t bufferSize) = 0;
};
