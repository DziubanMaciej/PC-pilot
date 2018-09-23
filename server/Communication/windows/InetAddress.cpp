#include "Communication/InetAddress.h"
#include "Utils/ApplicationError.h"

#include <ws2tcpip.h>
#include <sstream>

std::string InetAddress::ipToString(int addressFamily, void *address) {
	char buffer[64] = {};
	auto result = InetNtop(addressFamily, address, buffer, sizeof(buffer));
	if (result == nullptr) {
		ApplicationError::unrecoverable("Address parse");
	}
	return std::string{ buffer };
}

std::string InetAddress::ipToString(int addressFamily, void *address, uint16_t port) {
	std::ostringstream result;
	result << ipToString(addressFamily, address) << ":" << port;
	return result.str();
}

std::string InetAddress::toString(bool appendPort) const {
	IN_ADDR inAddr;
	inAddr.S_un.S_addr = this->address;
	if (appendPort) {
		return ipToString(AF_INET, &inAddr, this->port);
	}
	else {
		return ipToString(AF_INET, &inAddr);
	}
}


std::unique_ptr<InetAddress> InetAddress::createAny(uint16_t port) {
	return std::make_unique<InetAddress>(INADDR_ANY, port);
}

std::unique_ptr<InetAddress> InetAddress::createLoopback(uint16_t port) {
	return std::make_unique<InetAddress>(INADDR_LOOPBACK, port);

}

std::unique_ptr<InetAddress> InetAddress::createBroadcast(uint16_t port) {
	return std::make_unique<InetAddress>(INADDR_BROADCAST, port);
}

std::unique_ptr<InetAddress> InetAddress::createFromString(const std::string & address, uint16_t port) {
	uint32_t ip;
	auto result = InetPton(AF_INET, address.c_str(), &ip);
	if (result != 1) {
		ApplicationError::exception("InetAddress parse");
	}
	return std::make_unique<InetAddress>(ip, port);
}

std::ostream& operator<<(std::ostream& out, const InetAddress& address) {
	return out << address.toString(true);
}
