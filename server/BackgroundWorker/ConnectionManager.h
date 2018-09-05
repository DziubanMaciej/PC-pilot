#pragma once

#include "Communication/InetAddress.h"
#include "Communication/Messages.h"
#include "InputSimulator/InputSimulator.h"
#include "Utils/BlockingQueue.h"
#include "Utils/InterruptibleThread.h"

class ConnectionManager {
	enum class ConnectionManagerMessageType {
		CONNECTION_REQUEST,
		DISCONNECT,
		KEEP_ALIVE
	};

	using ConnectionManagerMessage = std::tuple<ConnectionManagerMessageType, InetAddress>;

	class KeepAliveSender : public InterruptibleThreadAdapter<ConnectionManager> {
		void onUpdate(ConnectionManager &connectionManager) override;
	};

	class KeepAliveReceiver : public InterruptibleThreadAdapter<ConnectionManager> {
		void onUpdate(ConnectionManager &connectionManager) override;
		void onUpdateConnected(ConnectionManager &connectionManager);
		void onUpdateUnconnected(ConnectionManager &connectionManager);
		ConnectionManagerMessage connectionManagerMessageBuffer;
	};

	friend class KeepAliveSender;
	friend class keepAliveReceiver;

public:
	ConnectionManager(BlockingQueue<ClientMessage> &toSendMessages, InputSimulator &inputSimulator);

	void start();
	void interrupt();
	void join();

	bool isConnected() const;
	const std::unique_ptr<InetAddress>& getConnectedAddress() const;

	void notifyConnectionRequest(const InetAddress &address);
	void notifyDisconnect(const InetAddress &address);
	void notifyKeepAlive(const InetAddress &address);

private:
	BlockingQueue<ClientMessage> &toSendMessages;
	InputSimulator &inputSimulator;

	std::unique_ptr<InetAddress> connectedAddress;
	BlockingQueue<ConnectionManagerMessage> connectionManagerMessages;
	KeepAliveReceiver keepAliveReceiver;
	KeepAliveSender keepAliveSender;
};
