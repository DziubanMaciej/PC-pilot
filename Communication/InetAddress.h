#pragma once

#pragma once

#include <string>
#include <memory>

class InetAddress {
public:
	InetAddress(uint32_t address, short port) : address(address), port(port) {}
	virtual ~InetAddress() {}

	uint32_t getAddress() const { return address; }
	short getPort() const { return port; }

private:
	const uint32_t address;
	const short port;
};
