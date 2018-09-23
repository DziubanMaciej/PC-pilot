#pragma once

#include "Application/InputProcessor.h"
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
	using ApplicationInputProcessor = InputProcessor<void, Application&>;
public:
	Application();
	Application(const Application&) = delete;
	~Application();
	void run(SocketContext &socketContext, InputSimulator &inputSimulator);

private:
	// run() submethods
	void validateIfFirstRun();
	void clearQueues();
	void createSockets(SocketContext &socketContext);
	void startThreads(InputSimulator &inputSimulator);
	void handleInputs();

	// Tear down methods
	void interruptThreads();
	void joinThreads();

	// Input processor
	ApplicationInputProcessor inputProcessor;
	static ApplicationInputProcessor::InputEntries createInputEntries();
	static void emptyCommandHandler(Application& application);
	static void unknownCommandHandler(Application& application);
	static void printStatus(Application& application);
	static void printHelp(Application& application);
	static void disconnect(Application& application);
	static void exit(Application& application);

	// Flags
	std::atomic_bool runCalled = false;
	bool exitCalled = false;

	// Message queues
	BlockingQueue<ServerMessage> receivedMessages;
	BlockingQueue<ClientMessage> toSendMessages;

	// Communication endpoints
	std::unique_ptr<InetAddress> address;
	std::unique_ptr<ConnectionlessSocket> receiveSocket;
	std::unique_ptr<ConnectionlessSocket> transmitSocket;

	// Background workers
	ConnectionManager connectionManager;
	Receiver receiver;
	Processor processor;
	Transmitter transmitter;
};