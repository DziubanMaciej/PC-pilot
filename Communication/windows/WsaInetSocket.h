#pragma once

#include "Communication/windows/WsaSocket.h"

#include <string>


class WsaInetSocket : public WsaSocket {
public:
	WsaInetSocket(const InetAddress &address, bool reusable);
	bool isConnected() const override;

	void listen() override;
	void accept() override;
	void connect(InetAddress &address) override;

	void send(const std::string &message) override;
	std::string receive(size_t bufferSize) override;

private:
	bool connected;
	SOCKET connectedSocket;
	sockaddr_in connectedAddress;
};
