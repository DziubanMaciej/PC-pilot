#pragma once

#include "BackgroundWorker/Transmitter.h"
#include "BackgroundWorker/Processor.h"
#include "BackgroundWorker/Receiver.h"
#include "BackgroundWorker/ConnectionManager.h"
#include "Communication/InetAddress.h"
#include "Communication/SocketContext.h"
#include "InputSimulator/InputSimulator.h"
#include "Utils/Constants.h"
#include "Utils/Logger.h"

#include <iostream> 
#include <memory>
#include <exception>
#include <string>
#include <thread>
#include <vector>

std::unique_ptr<SocketContext> context = SocketContext::create();
std::unique_ptr<InputSimulator> inputSimulator = InputSimulator::create();

int main() {
	BlockingQueue<ServerMessage> receivedMessages;
	BlockingQueue<ClientMessage> toSendMessages;

	ConnectionManager connectionManager;
	Receiver receiver;
	Processor processor;
	Transmitter transmitter;

	const auto address = InetAddress::createFromString(context->getInetAddresses(true, false, false)[0], 9999);
	auto receiveSocket = context->getInetSocket(*address, true, false);
	auto transmitSocket = context->getInetSocket(*address, true, true);

	connectionManager.start(toSendMessages, *inputSimulator);
	receiver.start(receivedMessages, *receiveSocket);
	processor.start(receivedMessages, toSendMessages, connectionManager, *inputSimulator);
	transmitter.start(toSendMessages, *transmitSocket);

	Logger::log("Press enter to interrupt threads and exit");
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

