#pragma once
#include <iostream> 
#include <memory>
#include <exception>
#include <string>
#include <thread>
#include <vector>

#include "Communication/windows/Wsa.h"
#include "InputSimulator/WindowsInputSimulator.h"
#include "Communication/InetAddress.h"
#include "Utils/Constants.h"

std::unique_ptr<SocketContext> context = std::make_unique<Wsa>();
std::unique_ptr<InputSimulator> inputSimulator = std::make_unique<WindowsInputSimulator>();

void receiver(const std::string &address) {
	auto myAddress = context->getInetAddress(address, 8888);
	auto mySocket = context->getInetSocket(*myAddress, false);
	std::cout << "RECEIVER: receiving...\n";

	std::vector<unsigned char> result;
	if (mySocket->receive(result, Constants::MAX_MESSAGE_SIZE, 100)) {
		std::string str{ (char*)result.data() };
		std::cout << "RECEIVER: received \"" << str << "\"\n";
	}
	else {
		std::cout << "RECEIVER: timeout\n";
	}
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

