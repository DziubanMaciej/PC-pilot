#pragma once

#include "Communication/ConnectionlessSocket.h"

#include <WinSock2.h>

class UdpSocket : public ConnectionlessSocket {
public:
	UdpSocket(const InetAddress &address, bool reusable, bool broadcast);
	~UdpSocket() override;

	void bind(const InetAddress &address, bool reusable, bool broadcast) override;
	void send(const std::vector<unsigned char> &message, const InetAddress &address) override;
	bool receive(std::vector<unsigned char> &result, InetAddress& senderAddress, size_t bufferSize, uint32_t timeoutMs) override;

private:
	const SOCKET socket;
};
