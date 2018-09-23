#include "Application/Application.h"

#include "Application/InputProcessor.h"
#include "Communication/SocketContext.h"
#include "InputSimulator/InputSimulator.h"
#include "Utils/ApplicationError.h"
#include "Utils/Logger.h"

void Application::run(SocketContext &socketContext, InputSimulator &inputSimulator) {
	validateIfFirstRun();
	clearQueues();
	createSockets(socketContext);
	startThreads(inputSimulator);
	handleInputs();
}

std::string Application::getStatus() {
	std::string result;

	if (connectionManager.isConnected()) {
		result.append("Connected to ");
		result.append(InetAddress::ipToString(*connectionManager.getConnectedAddress(), true));
	}
	else {
		result.append("Waiting for connection...");
	}

	return result;
}

std::string Application::getAddress() {
	std::string result = "This host's address is ";
	result.append(InetAddress::ipToString(*this->address, true));
	return result;
}

void Application::exit() {
	this->exitCalled = true;
}

Application::~Application() {
	interruptThreads();
	joinThreads();
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
	Logger::log("Sockets established on ", InetAddress::ipToString(*this->address, true));
}

void Application::startThreads(InputSimulator &inputSimulator) {
	this->connectionManager.start(this->toSendMessages, inputSimulator);
	this->receiver.start(this->receivedMessages, *this->receiveSocket);
	this->processor.start(this->receivedMessages, this->toSendMessages, this->connectionManager, inputSimulator);
	this->transmitter.start(this->toSendMessages, *this->transmitSocket);
}

void Application::handleInputs() {
	InputProcessor::printHelp(*this);
	while (!this->exitCalled) {
		std::string command;
		std::getline(std::cin, command);
		InputProcessor::call(*this, command);
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
