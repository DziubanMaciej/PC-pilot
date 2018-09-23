#pragma once

#include "Communication/SocketContext.h"

struct sockaddr_in;
struct _IP_ADAPTER_ADDRESSES_LH;
struct InetAddress;

class Wsa : public SocketContext {
public:
	Wsa();
	~Wsa() override;

	std::unique_ptr<ConnectionlessSocket> getInetSocket(const InetAddress &address, bool reusable, bool broadcast) override;

	std::vector<std::string> getInetAddresses(bool ipv4, bool ipv6, bool loopback) override;
	static sockaddr_in convertInetAddress(const InetAddress &address);
private:
	using Adapter = _IP_ADAPTER_ADDRESSES_LH;
	static std::unique_ptr<Adapter[]> getAdapters();
};
