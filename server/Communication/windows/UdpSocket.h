#pragma once

#include "Communication/ConnectionlessSocket.h"

#include <WinSock2.h>

class UdpSocket : public ConnectionlessSocket {
public:
	UdpSocket(const InetAddress &address, bool reusable);
	~UdpSocket() override;

	void bind(const InetAddress &address, bool reusable) override;
	void send(const std::string &message, const InetAddress &address) override;
	std::string receive(size_t bufferSize) override;

private:
	const SOCKET socket;
};
