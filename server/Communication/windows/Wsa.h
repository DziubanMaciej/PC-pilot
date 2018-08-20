#pragma once

#include "Communication/SocketContext.h"

struct sockaddr_in;
struct _IP_ADAPTER_ADDRESSES_LH;

class Wsa : public SocketContext {
public:
	Wsa();
	~Wsa();
	
	std::unique_ptr<InetAddress> getInetAddressAny(short port) override;
	std::unique_ptr<InetAddress> getInetAddressLoopback(short port) override;
	std::unique_ptr<InetAddress> getInetAddress(uint32_t address, short port) override;
	std::unique_ptr<InetAddress> getInetAddress(const std::string &address, short port) override;
	std::unique_ptr<ConnectionlessSocket> getInetSocket(const InetAddress &address, bool reusable) override;

	std::vector<std::string> getInetAddresses(bool ipv4, bool ipv6, bool loopback) override;

	static sockaddr_in convertInetAddress(const InetAddress &address);

private:
	using Adapter = _IP_ADAPTER_ADDRESSES_LH;
	static std::unique_ptr<Adapter[]> getAdapters();
	static std::string ipToString(int addressFamily, void *address);
};
