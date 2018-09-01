#include "Communication/InetAddress.h"
#include "Utils/ApplicationError.h"

#include <ws2tcpip.h>

std::string InetAddress::ipToString(int addressFamily, const void * address) {
	char buffer[64] = {};
	auto result = InetNtop(addressFamily, address, buffer, sizeof(buffer));
	if (result == nullptr) {
		ApplicationError::unrecoverable("Address parse");
	}
	return std::string{ buffer };
}

std::string InetAddress::ipToString(const InetAddress & address) {
	return ipToString(AF_INET, &address.address);
}

std::unique_ptr<InetAddress> InetAddress::createAny(short port) {
	return std::make_unique<InetAddress>(INADDR_ANY, port);
}

std::unique_ptr<InetAddress> InetAddress::createLoopback(short port) {
	return std::make_unique<InetAddress>(INADDR_LOOPBACK, port);

}

std::unique_ptr<InetAddress> InetAddress::createFromString(const std::string & address, short port) {
	uint32_t ip;
	auto result = InetPton(AF_INET, address.c_str(), &ip);
	if (result != 1) {
		ApplicationError::exception("InetAddress parse");
	}
	return std::make_unique<InetAddress>(ip, port);
}

std::ostream& operator<<(std::ostream& out, const InetAddress& address) {
	return out << InetAddress::ipToString(address) << ":" << address.port;
}
