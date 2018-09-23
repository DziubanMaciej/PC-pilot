#include "Application/Application.h"

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
	const auto address = InetAddress::createFromString(socketContext.getInetAddresses(true, false, false)[0], 9999);
	this->receiveSocket = socketContext.getInetSocket(*address, true, false);
	this->transmitSocket = socketContext.getInetSocket(*address, true, true);
}

void Application::startThreads(InputSimulator &inputSimulator) {
	this->connectionManager.start(this->toSendMessages, inputSimulator);
	this->receiver.start(this->receivedMessages, *this->receiveSocket);
	this->processor.start(this->receivedMessages, this->toSendMessages, this->connectionManager, inputSimulator);
	this->transmitter.start(this->toSendMessages, *this->transmitSocket);
}

void Application::handleInputs() {
	Logger::log("Press enter to interrupt threads and exit");
	std::cin.get();
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
