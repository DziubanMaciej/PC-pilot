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

void Application::run(SocketContext &socketContext, InputSimulator &inputSimulator, uint16_t port) {
	validateIfFirstRun();
	clearQueues();
	createSockets(socketContext, port);
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

void Application::createSockets(SocketContext &socketContext, uint16_t port) {
	this->address = InetAddress::createFromString(socketContext.getInetAddresses(true, false, false)[0], port);
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
		{ "disconnect", Application::disconnect },
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
	std::vector<std::string> tokens;
	tokens.push_back("This host's address is ");
	tokens.push_back(application.address->toString(true));
	tokens.push_back("\n");

	if (application.connectionManager.isConnected()) {
		tokens.push_back("Connected to ");
		tokens.push_back(application.connectionManager.getConnectedAddress().toString(true));
	}
	else {
		tokens.push_back("No remote device connected");
	}

	Logger::log.dumpContainer(tokens.begin(), tokens.end(), false);
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

void Application::disconnect(Application &application) {
	auto lock = application.connectionManager.lock();
	if (!application.connectionManager.isConnected()) {
		Logger::log("Already disconnected");
		return;
	}

	auto &connectedAddress = application.connectionManager.getConnectedAddress();
	application.toSendMessages.push(ClientMessage::createMessageDisconnect(connectedAddress));
	application.connectionManager.disconnect();
}

void Application::exit(Application &application) {
	application.exitCalled = true;
}
