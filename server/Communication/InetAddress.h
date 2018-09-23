#pragma once

#include <memory>
#include <string>

struct InetAddress {
	InetAddress() : InetAddress(0, 0) {}
	InetAddress(uint32_t address, unsigned short port) : address(address), port(port) {}

	uint32_t address;
	unsigned short port;

	static std::string ipToString(int addressFamily, void *address);
	static std::string ipToString(int addressFamily, void *address, short port);
	static std::string ipToString(const InetAddress &address, bool appendPort);

	static std::unique_ptr<InetAddress> createAny(short port);
	static std::unique_ptr<InetAddress> createLoopback(short port);
	static std::unique_ptr<InetAddress> createBroadcast(short port);
	static std::unique_ptr<InetAddress> createFromString(const std::string &address, short port);
};

namespace {
	class ostream;
}

std::ostream& operator<<(std::ostream&, const InetAddress&);
