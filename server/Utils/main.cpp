#pragma once
#include <iostream> 
#include <memory>
#include <exception>
#include <string>
#include <thread>

#include "Communication/windows/Wsa.h"
#include "InputSimulator/WindowsInputSimulator.h"
#include "Communication/InetAddress.h"

std::unique_ptr<SocketContext> context = std::make_unique<Wsa>();
std::unique_ptr<InputSimulator> inputSimulator = std::make_unique<WindowsInputSimulator>();

void receiver(const std::string &address) {
	auto myAddress = context->getInetAddress(address, 8888);
	auto mySocket = context->getInetSocket(*myAddress, false);
	std::cout << "RECEIVER: receiving...\n";
	std::cout << "RECEIVER: received \"" << mySocket->receive(64) << "\"\n";
}

void sender(const std::string &address) {
	auto myAddress = context->getInetAddress(address, 9999);
	auto theirAddress = context->getInetAddress(address, 8888);
	auto mySocket = context->getInetSocket(*myAddress, false);
	inputSimulator->sleepMs(500);
	mySocket->send(":)", *theirAddress);
	std::cout << "SENDER: sent\n";
}

int main() {
	const auto addresses = context->getInetAddresses(true, false, false);
	const auto address = addresses[0];

	std::thread receiverThread(receiver, address);
	std::thread senderThread(sender, address);

	receiverThread.join();
	senderThread.join();

	system("pause");
}

