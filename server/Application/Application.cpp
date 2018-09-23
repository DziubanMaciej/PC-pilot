#include "Application/Application.h"

#include "Application/InputProcessor.h"
#include "Communication/SocketContext.h"
#include "InputSimulator/InputSimulator.h"
#include "Utils/ApplicationError.h"
#include "Utils/Logger.h"

Application::Application() : inputProcessor(createInputEntries(), (Application::ApplicationInputProcessor::InputHandler) unknownCommandHandler) {}

Application::~Application() {
	interruptThreads();
	joinThreads();
}

void Application::run(SocketContext &socketContext, InputSimulator &inputSimulator) {
	validateIfFirstRun();
	clearQueues();
	createSockets(socketContext);
	startThreads(inputSimulator);
	handleInputs();
}

void Application::validateIfFirstRun() {
	bool alreadyRun = this->runCalled.exchange(true);
	if (alreadyRun) {
		ApplicationError::exception("Application::run() may be called only once");
	}
}

void Application::clearQueues() {
	this->toSendMessages.clear();
	this->receivedMessages.clear();
}

void Application::createSockets(SocketContext &socketContext) {
	this->address = InetAddress::createFromString(socketContext.getInetAddresses(true, false, false)[0], 9999);
	this->receiveSocket = socketContext.getInetSocket(*this->address, true, false);
	this->transmitSocket = socketContext.getInetSocket(*this->address, true, true);
	Logger::log("Sockets established on ", this->address->toString(true));
}

void Application::startThreads(InputSimulator &inputSimulator) {
	this->connectionManager.start(this->toSendMessages, inputSimulator);
	this->receiver.start(this->receivedMessages, *this->receiveSocket);
	this->processor.start(this->receivedMessages, this->toSendMessages, this->connectionManager, inputSimulator);
	this->transmitter.start(this->toSendMessages, *this->transmitSocket);
}

void Application::handleInputs() {
	Application::printHelp(*this);
	while (!this->exitCalled) {
		std::string command;
		std::getline(std::cin, command);
		inputProcessor.call(command, *this);
	}
}

void Application::interruptThreads() {
	this->transmitter.interrupt();
	this->processor.interrupt();
	this->receiver.interrupt();
	this->connectionManager.interrupt();
}

void Application::joinThreads() {
	this->transmitter.join();
	this->processor.join();
	this->receiver.join();
	this->connectionManager.join();
}

Application::ApplicationInputProcessor::InputEntries Application::createInputEntries() {
	return ApplicationInputProcessor::InputEntries {
		{ "", Application::emptyCommandHandler },
		{ "status", Application::printStatus },
		{ "help", Application::printHelp },
		{ "address", Application::printAddress },
		{ "exit", Application::exit }
	};
}

void Application::emptyCommandHandler(Application &application) {
	// No operation
}

void Application::unknownCommandHandler(Application &application) {
	Logger::log("Unknown command provided. Type \"help\" to see available commands.\n");
}

void Application::printStatus(Application &application) {
	if (application.connectionManager.isConnected()) {
		Logger::log("Connected to ", application.connectionManager.getConnectedAddress().toString(true));
	}
	else {
		Logger::log("Waiting for connection...");
	}
}

void Application::printHelp(Application &application) {
	std::vector<std::string> tokens;
	tokens.push_back("Available commands:");

	for (const auto& entry : application.inputProcessor.inputEntries) {
		if (!entry.first.empty()) {
			std::string line = "     - ";
			line.append(entry.first);
			tokens.push_back(line);
		}
	}

	Logger::log.dumpContainer(tokens.begin(), tokens.end());
}

void Application::printAddress(Application &application) {
	Logger::log("This host's address is ", application.address->toString(true));
}

void Application::exit(Application &application) {
	application.exitCalled = true;
}
