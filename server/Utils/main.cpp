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

#include "BackgroundWorker/Transmitter.h"
#include "BackgroundWorker/Processor.h"
#include "BackgroundWorker/Receiver.h"
#include "BackgroundWorker/ConnectionManager.h"

int main() {
	BlockingQueue<ServerMessage> receivedMessages;
	BlockingQueue<ClientMessage> toSendMessages;

	ConnectionManager connectionManager;
	Receiver receiver;
	Processor processor;
	Transmitter transmitter;

	const auto address = context->getInetAddress(context->getInetAddresses(true, false, false)[0], 9999);
	auto receiveSocket = context->getInetSocket(*address, true);
	auto transmitSocket = context->getInetSocket(*address, true);

	receiver.start(receivedMessages, *receiveSocket);
	processor.start(receivedMessages, toSendMessages, connectionManager, *inputSimulator);
	transmitter.start(toSendMessages, *transmitSocket);

	inputSimulator->sleepMs(2000);
	receiver.interrupt();
	processor.interrupt();
	transmitter.interrupt();

	receiver.join();
	processor.join();
	transmitter.join();
	system("pause");
}

