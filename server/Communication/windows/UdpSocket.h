#pragma once

#include "Communication/ConnectionlessSocket.h"

#include <WinSock2.h>

class UdpSocket : public ConnectionlessSocket {
public:
	UdpSocket(const InetAddress &address, bool reusable);
	~UdpSocket() override;

	void bind(const InetAddress &address, bool reusable) override;
	void send(const std::string &message, const InetAddress &address) override;
	bool receive(std::vector<unsigned char> &result, InetAddress& senderAddress, size_t bufferSize, uint32_t timeoutMs) override;

private:
	const SOCKET socket;
};
