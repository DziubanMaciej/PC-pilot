#pragma once
#include <iostream> 
#include <memory>
#include <exception>
#include <string>

#include "Communication/windows/Wsa.h"
#include "Communication/InetAddress.h"
#include "Communication/ConnectionOrientedSocket.h"

#include <ws2tcpip.h>
uint32_t parseIp(const std::string & str) {
	uint32_t ip;
	auto result = InetPton(AF_INET, str.c_str(), &ip);
	if (result != 1) throw std::exception("parseIp");
	return ip;
}

int main() {
	std::cout << "1. Receive\n2.Send\n: ";
	char c = (char) std::cin.get();
	const auto ipAddress = parseIp("192.168.1.101");
	if (c == '1') {
		std::unique_ptr<SocketContext> context = std::make_unique<Wsa>();
		auto myAddress = context->getInetAddress(ipAddress, 8888);
		auto mySocket = context->getInetSocket(*myAddress, false);
		mySocket->listen();
		mySocket->accept();
		std::cout << mySocket->receive(10);
	}
	else if (c == '2') {
		std::unique_ptr<SocketContext> context = std::make_unique<Wsa>();
		auto myAddress = context->getInetAddress(ipAddress, 9999);
		auto theirAddress = context->getInetAddress(ipAddress, 8888);
		auto mySocket = context->getInetSocket(*myAddress, false);
		mySocket->connect(*theirAddress);
		mySocket->send(":)");
	}
	else {
		std::cout << "Unknown input\n";
	}


	std::cout << "End\n";
	system("pause");
}

