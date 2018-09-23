#include "ConnectionManager.h"
#include "Utils/ApplicationError.h"
#include "Utils/Constants.h"
#include "Utils/Logger.h"

void ConnectionManager::KeepAliveSender::onUpdate(ConnectionManager &connectionManager, BlockingQueue<ClientMessage> &toSendMessages, InputSimulator &inputSimulator) {
	auto lock = connectionManager.lock();
	if (!connectionManager.isConnected()) {
		auto port = Constants::AVAILABLE_CLIENT_PORTS[0];
		toSendMessages.push(ClientMessage::createMessageAdvertise(*InetAddress::createBroadcast(port)));
	}
	else {
		toSendMessages.push(ClientMessage::createMessageKeepAlive(connectionManager.getConnectedAddress()));
	}

	lock.unlock();
	inputSimulator.sleepMs(Constants::KEEP_ALIVE_SEND_RATE_MS);
}

void ConnectionManager::KeepAliveReceiver::onUpdate(ConnectionManager &connectionManager) {
	auto lock = connectionManager.lock();
	auto connected = connectionManager.isConnected();
	lock.unlock();

	if (connected) onUpdateConnected(connectionManager);
	else onUpdateUnconnected(connectionManager);
}

void ConnectionManager::KeepAliveReceiver::onUpdateConnected(ConnectionManager &connectionManager) {
	// TODO this implementation allows any CONNECTION_REQUEST and KEEP_ALIVE to sustain connection
	if (connectionManager.connectionManagerMessages.popAndGet(this->connectionManagerMessageBuffer, std::chrono::milliseconds(Constants::KEEP_ALIVE_TIMEOUT_MS))) {
		// Lock and check if precondition hasn't changed
		auto lock = connectionManager.lock();
		if (!connectionManager.isConnected()) { // Could have been disconnected while getting message
			return;
		}

		switch (std::get<ConnectionManagerMessageType>(this->connectionManagerMessageBuffer)) {
			case ConnectionManagerMessageType::CONNECTION_REQUEST:
				return;
			case ConnectionManagerMessageType::DISCONNECT:
				break;
			case ConnectionManagerMessageType::KEEP_ALIVE:
				return;
			default:
				ApplicationError::exception("Unknown enum value");
		}
	}

	// popAndGet has timed out or received disconnect message - disconnecting
	connectionManager.disconnect();
}

void ConnectionManager::KeepAliveReceiver::onUpdateUnconnected(ConnectionManager &connectionManager) {
	// Get message or time out
	if (!connectionManager.connectionManagerMessages.popAndGet(this->connectionManagerMessageBuffer, std::chrono::milliseconds(Constants::MANUAL_LOOP_BACK_RATE_MS))) {
		return;
	}

	// Lock and check if precondition hasn't changed
	auto lock = connectionManager.lock();
	if (connectionManager.isConnected()) {
		return;
	}

	// Connect if message was connection request
	if (std::get<ConnectionManagerMessageType>(this->connectionManagerMessageBuffer) == ConnectionManagerMessageType::CONNECTION_REQUEST) {
		auto connectedAddress = std::get<InetAddress>(this->connectionManagerMessageBuffer);
		connectionManager.connect(connectedAddress);
	}
}

void ConnectionManager::start(BlockingQueue<ClientMessage> &toSendMessages, InputSimulator &inputSimulator) {
	auto lock = this->lock();
	keepAliveReceiver.start(*this);
	keepAliveSender.start(*this, toSendMessages, inputSimulator);
}

void ConnectionManager::interrupt() {
	auto lock = this->lock();
	keepAliveReceiver.interrupt();
	keepAliveSender.interrupt();
}

void ConnectionManager::join() {
	auto lock = this->lock();
	keepAliveReceiver.join();
	keepAliveSender.join();
}

void ConnectionManager::connect(const InetAddress & address) {
	auto lock = this->lock();
	this->connectedAddress = std::make_unique<InetAddress>(address);
	Logger::log("Connected to: ", *this->connectedAddress);
}

void ConnectionManager::disconnect() {
	auto lock = this->lock();
	Logger::log("Disconnected from: ", *this->connectedAddress);
	this->connectedAddress.reset();
}

bool ConnectionManager::isConnected() const {
	return bool(this->connectedAddress);
}

const InetAddress& ConnectionManager::getConnectedAddress() const {
	return *this->connectedAddress;
}

void ConnectionManager::notifyConnectionRequest(const InetAddress & address) {
	this->connectionManagerMessages.push(std::make_tuple(ConnectionManagerMessageType::CONNECTION_REQUEST, address));
}

void ConnectionManager::notifyDisconnect(const InetAddress &address) {
	this->connectionManagerMessages.push(std::make_tuple(ConnectionManagerMessageType::DISCONNECT, address));
}

void ConnectionManager::notifyKeepAlive(const InetAddress & address) {
	this->connectionManagerMessages.push(std::make_tuple(ConnectionManagerMessageType::KEEP_ALIVE, address));
}
