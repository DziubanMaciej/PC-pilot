#pragma once

#include <memory>

struct InetAddress {
	InetAddress() : InetAddress(0, 0) {}
	InetAddress(uint32_t address, unsigned short port) : address(address), port(port) {}

	uint32_t address;
	unsigned short port;
};
