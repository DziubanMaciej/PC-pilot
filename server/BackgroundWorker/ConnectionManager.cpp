#include "ConnectionManager.h"
#include "Utils/ApplicationError.h"
#include "Utils/Constants.h"
#include "Utils/Logger.h"

void ConnectionManager::KeepAliveSender::onUpdate(ConnectionManager &connectionManager, BlockingQueue<ClientMessage> &toSendMessages, InputSimulator &inputSimulator) {
	auto lock = connectionManager.lock();
	if (!connectionManager.isConnected()) {
		short port = Constants::AVAILABLE_CLIENT_PORTS[0];
		toSendMessages.push(ClientMessage::createMessageAdvertise(*InetAddress::createBroadcast(port)));
	}
	else {
		toSendMessages.push(ClientMessage::createMessageKeepAlive(*connectionManager.getConnectedAddress()));
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

	auto lock = connectionManager.lock();
	Logger::log("Disconnected from: ", *connectionManager.connectedAddress);
	connectionManager.connectedAddress.reset();
}

void ConnectionManager::KeepAliveReceiver::onUpdateUnconnected(ConnectionManager &connectionManager) {
	if (!connectionManager.connectionManagerMessages.popAndGet(this->connectionManagerMessageBuffer, std::chrono::milliseconds(Constants::MANUAL_LOOP_BACK_RATE_MS))) {
		return;
	}

	auto lock = connectionManager.lock();
	if (connectionManager.isConnected()) { // Could have been connected while getting message
		return;
	}

	if (std::get<ConnectionManagerMessageType>(this->connectionManagerMessageBuffer) == ConnectionManagerMessageType::CONNECTION_REQUEST) {
		connectionManager.connectedAddress = std::make_unique<InetAddress>(std::get<InetAddress>(this->connectionManagerMessageBuffer));
		Logger::log("Connected to: ", *connectionManager.connectedAddress);
	}
}

void ConnectionManager::start(BlockingQueue<ClientMessage> &toSendMessages, InputSimulator &inputSimulator) {
	keepAliveReceiver.start(*this);
	keepAliveSender.start(*this, toSendMessages, inputSimulator);
}

void ConnectionManager::interrupt() {
	keepAliveReceiver.interrupt();
	keepAliveSender.interrupt();
}

void ConnectionManager::join() {
	keepAliveReceiver.join();
	keepAliveSender.join();
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

void ConnectionManager::notifyDisconnect(const InetAddress &address) {
	this->connectionManagerMessages.push(std::make_tuple(ConnectionManagerMessageType::DISCONNECT, address));
}

void ConnectionManager::notifyKeepAlive(const InetAddress & address) {
	this->connectionManagerMessages.push(std::make_tuple(ConnectionManagerMessageType::KEEP_ALIVE, address));
}
