#pragma once

#include "BackgroundWorker/Transmitter.h"
#include "BackgroundWorker/Processor.h"
#include "BackgroundWorker/Receiver.h"
#include "BackgroundWorker/ConnectionManager.h"
#include "Utils/BlockingQueue.h"

#include <atomic>
#include <memory>

class SocketContext;
class InputSimulator;

class Application {
public:
	Application() = default;
	~Application();
	Application(const Application&) = delete;
	void run(SocketContext &socketContext, InputSimulator &inputSimulator);
	std::string getStatus();
	void exit();

private:
	void validateIfFirstRun();
	void clearQueues();
	void createSockets(SocketContext &socketContext);
	void startThreads(InputSimulator &inputSimulator);
	void handleInputs();

	void interruptThreads();
	void joinThreads();

	std::atomic_bool runCalled = false;
	bool exitCalled = false;

	BlockingQueue<ServerMessage> receivedMessages;
	BlockingQueue<ClientMessage> toSendMessages;

	std::unique_ptr<ConnectionlessSocket> receiveSocket;
	std::unique_ptr<ConnectionlessSocket> transmitSocket;

	ConnectionManager connectionManager;
	Receiver receiver;
	Processor processor;
	Transmitter transmitter;
};