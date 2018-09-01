#pragma once

#include "BackgroundWorker/Transmitter.h"
#include "BackgroundWorker/Processor.h"
#include "BackgroundWorker/Receiver.h"
#include "BackgroundWorker/ConnectionManager.h"
#include "Communication/windows/Wsa.h"
#include "Communication/InetAddress.h"
#include "InputSimulator/WindowsInputSimulator.h"
#include "Utils/Constants.h"
#include "Utils/Logger.h"

#include <iostream> 
#include <memory>
#include <exception>
#include <string>
#include <thread>
#include <vector>

std::unique_ptr<SocketContext> context = std::make_unique<Wsa>();
std::unique_ptr<InputSimulator> inputSimulator = std::make_unique<WindowsInputSimulator>();

int main() {
	BlockingQueue<ServerMessage> receivedMessages;
	BlockingQueue<ClientMessage> toSendMessages;

	ConnectionManager connectionManager{ toSendMessages, *inputSimulator };
	Receiver receiver;
	Processor processor;
	Transmitter transmitter;

	const auto address = context->getInetAddress(context->getInetAddresses(true, false, false)[0], 9999);
	auto receiveSocket = context->getInetSocket(*address, true);
	auto transmitSocket = context->getInetSocket(*address, true);

	connectionManager.start();
	receiver.start(receivedMessages, *receiveSocket);
	processor.start(receivedMessages, toSendMessages, connectionManager, *inputSimulator);
	transmitter.start(toSendMessages, *transmitSocket);

	Logger::log("Press enter to interrupt threads and exit\n");
	std::cin.get();

	connectionManager.interrupt();
	receiver.interrupt();
	processor.interrupt();
	transmitter.interrupt();

	connectionManager.join();
	receiver.join();
	processor.join();
	transmitter.join();
}

