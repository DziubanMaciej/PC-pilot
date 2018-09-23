#pragma once

#include <memory>
#include <string>

struct InetAddress {
	InetAddress() : InetAddress(0, 0) {}
	InetAddress(uint32_t address, uint16_t port) : address(address), port(port) {}

	uint32_t address;
	uint16_t port;

	static std::string ipToString(int addressFamily, void *address);
	static std::string ipToString(int addressFamily, void *address, uint16_t port);
	static std::string ipToString(const InetAddress &address, bool appendPort);

	static std::unique_ptr<InetAddress> createAny(uint16_t port);
	static std::unique_ptr<InetAddress> createLoopback(uint16_t port);
	static std::unique_ptr<InetAddress> createBroadcast(uint16_t port);
	static std::unique_ptr<InetAddress> createFromString(const std::string &address, uint16_t port);
};

namespace {
	class ostream;
}

std::ostream& operator<<(std::ostream&, const InetAddress&);
