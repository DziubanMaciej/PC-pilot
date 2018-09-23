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

std::string InetAddress::ipToString(int addressFamily, void *address, short port) {
	std::ostringstream result;
	result << ipToString(addressFamily, address) << ":" << port;
	return result.str();
}

std::string InetAddress::ipToString(const InetAddress &address, bool appendPort) {
	IN_ADDR inAddr;
	inAddr.S_un.S_addr = address.address;
	if (appendPort) {
		return ipToString(AF_INET, &inAddr, address.port);
	}
	else {
		return ipToString(AF_INET, &inAddr);
	}
}

std::unique_ptr<InetAddress> InetAddress::createAny(short port) {
	return std::make_unique<InetAddress>(INADDR_ANY, port);
}

std::unique_ptr<InetAddress> InetAddress::createLoopback(short port) {
	return std::make_unique<InetAddress>(INADDR_LOOPBACK, port);

}

std::unique_ptr<InetAddress> InetAddress::createBroadcast(short port) {
	return std::make_unique<InetAddress>(INADDR_BROADCAST, port);
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
	return out << InetAddress::ipToString(address, true);
}
