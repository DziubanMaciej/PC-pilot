#pragma once
#include <iostream> 
#include <memory>
#include <exception>
#include <string>

#include "Communication/windows/Wsa.h"
#include "Communication/InetAddress.h"
#include "Communication/ConnectionOrientedSocket.h"

int main() {
	std::cout << "1. Receive\n2.Send\n: ";
	char c = (char) std::cin.get();
	const std::string address = "192.168.0.60";
	if (c == '1') {
		std::unique_ptr<SocketContext> context = std::make_unique<Wsa>();
		auto myAddress = context->getInetAddress(address, 8888);
		auto mySocket = context->getInetSocket(*myAddress, false);
		mySocket->listen();
		mySocket->accept();
		std::cout << mySocket->receive(10);
	}
	else if (c == '2') {
		std::unique_ptr<SocketContext> context = std::make_unique<Wsa>();
		auto myAddress = context->getInetAddress(address, 9999);
		auto theirAddress = context->getInetAddress(address, 8888);
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

