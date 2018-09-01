#include "ConnectionManager.h"
#include "Utils/ApplicationError.h"
#include "Utils/Logger.h"

void ConnectionManager::KeepAliveSender::onUpdate(ConnectionManager &connectionManager) {
	if (!connectionManager.isConnected()) {
		this->interrupt();
		return;
	}
	connectionManager.toSendMessages.push(ClientMessage::createMessageKeepAlive(*connectionManager.getConnectedAddress()));
	connectionManager.inputSimulator.sleepMs(1000); // TODO constant
}

void ConnectionManager::KeepAliveReceiver::onUpdate(ConnectionManager &connectionManager) {
	if (!connectionManager.isConnected()) {
		if (!connectionManager.connectionManagerMessages.popAndGet(this->connectionManagerMessageBuffer, std::chrono::milliseconds(100))) { // TODO constant
			return;
		}

		if (std::get<ConnectionManagerMessageType>(this->connectionManagerMessageBuffer) == ConnectionManagerMessageType::CONNECTION_REQUEST) {
			connectionManager.connectedAddress = std::make_unique<InetAddress>(std::get<InetAddress>(this->connectionManagerMessageBuffer));
			connectionManager.keepAliveSender.start(connectionManager);
			Logger::log("Connected to: ", *connectionManager.connectedAddress);
		}
	}
	else {
		if (!connectionManager.connectionManagerMessages.popAndGet(this->connectionManagerMessageBuffer, std::chrono::milliseconds(100000)) 
			|| std::get<ConnectionManagerMessageType>(this->connectionManagerMessageBuffer) != ConnectionManagerMessageType::KEEP_ALIVE) {    // TODO constant
				connectionManager.keepAliveSender.interrupt();
				connectionManager.keepAliveSender.join();
				Logger::log("Disconnected from: ", *connectionManager.connectedAddress);
				connectionManager.connectedAddress.reset();
		}
	}
}

ConnectionManager::ConnectionManager(BlockingQueue<ClientMessage>& toSendMessages, InputSimulator & inputSimulator) :
	toSendMessages(toSendMessages),
	inputSimulator(inputSimulator) {}

void ConnectionManager::start() {
	keepAliveReceiver.start(*this);
}

void ConnectionManager::interrupt() {
	keepAliveReceiver.interrupt();
	keepAliveSender.interrupt();
}

void ConnectionManager::join() {
	keepAliveReceiver.join();
	keepAliveSender.joinIfJoinable();
}

bool ConnectionManager::isConnected() const {
	return bool(this->connectedAddress);
}

const std::unique_ptr<InetAddress>& ConnectionManager::getConnectedAddress() const {
	return this->connectedAddress;
}

void ConnectionManager::notifyConnectionRequest(const InetAddress & address) {
	this->connectionManagerMessages.push(std::make_tuple(ConnectionManagerMessageType::CONNECTION_REQUEST, address));
}

void ConnectionManager::notifyKeepAlive(const InetAddress & address) {
	this->connectionManagerMessages.push(std::make_tuple(ConnectionManagerMessageType::KEEP_ALIVE, address));
}
