#pragma once

#include "Communication/InetAddress.h"
#include "Communication/Messages.h"
#include "InputSimulator/InputSimulator.h"
#include "Utils/BlockingQueue.h"
#include "Utils/InterruptibleThread.h"
#include <mutex>

class ConnectionManager {
	enum class ConnectionManagerMessageType {
		CONNECTION_REQUEST,
		DISCONNECT,
		KEEP_ALIVE
	};
	using ConnectionManagerMessage = std::tuple<ConnectionManagerMessageType, InetAddress>;

	class KeepAliveSender : public InterruptibleThreadAdapter<ConnectionManager, BlockingQueue<ClientMessage>, InputSimulator> {
		void onUpdate(ConnectionManager &connectionManager, BlockingQueue<ClientMessage> &toSendMessages, InputSimulator &inputSimulator) override;
	};

	class KeepAliveReceiver : public InterruptibleThreadAdapter<ConnectionManager> {
		void onUpdate(ConnectionManager &connectionManager) override;
		void onUpdateConnected(ConnectionManager &connectionManager);
		void onUpdateUnconnected(ConnectionManager &connectionManager);
		ConnectionManagerMessage connectionManagerMessageBuffer;
	};


public:
	ConnectionManager() = default;
	ConnectionManager(const ConnectionManager&) = delete;

	void start(BlockingQueue<ClientMessage> &toSendMessages, InputSimulator &inputSimulator);
	void interrupt();
	void join();

	void connect(const InetAddress &address);
	void disconnect();
	bool isConnected() const;
	const InetAddress& getConnectedAddress() const;

	void notifyConnectionRequest(const InetAddress &address);
	void notifyDisconnect(const InetAddress &address);
	void notifyKeepAlive(const InetAddress &address);

private:
	std::mutex mutex;
	std::unique_ptr<InetAddress> connectedAddress;
	BlockingQueue<ConnectionManagerMessage> connectionManagerMessages;
	KeepAliveReceiver keepAliveReceiver;
	KeepAliveSender keepAliveSender;

	std::unique_lock<std::mutex> lock() {
		return std::unique_lock<std::mutex> { this->mutex };
	}
};
