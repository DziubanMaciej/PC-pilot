#pragma once
#include <iostream> 
#include <memory>
#include <exception>
#include <string>
#include <thread>

#include "Communication/windows/Wsa.h"
#include "InputSimulator/WindowsInputSimulator.h"
#include "Communication/InetAddress.h"
#include "Communication/ConnectionOrientedSocket.h"

std::unique_ptr<SocketContext> context = std::make_unique<Wsa>();
std::unique_ptr<InputSimulator> inputSimulator = std::make_unique<WindowsInputSimulator>();

void receiver(const std::string &address) {
	auto myAddress = context->getInetAddress(address, 8888);
	auto mySocket = context->getInetSocket(*myAddress, false);

	mySocket->listen();
	mySocket->accept();
	inputSimulator->sleepMs(500);
	std::cout << "RECEIVER: received \"" << mySocket->receive(10) << "\"\n";
}

void sender(const std::string &address) {
	auto myAddress = context->getInetAddress(address, 9999);
	auto theirAddress = context->getInetAddress(address, 8888);
	auto mySocket = context->getInetSocket(*myAddress, false);

	mySocket->connect(*theirAddress);
	mySocket->send(":)");
	std::cout << "SENDER: sent\n";
}

int main() {
	const std::string address = "192.168.0.60";

	std::thread receiverThread(receiver, address);
	std::thread senderThread(sender, address);

	receiverThread.join();
	senderThread.join();

	system("pause");
}

