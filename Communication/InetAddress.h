#pragma once

#include <memory>

struct InetAddress {
	InetAddress(uint32_t address, short port) : address(address), port(port) {}
	const uint32_t address;
	const short port;
};
