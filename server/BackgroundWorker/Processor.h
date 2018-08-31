#pragma once

#include "BackgroundWorker/ConnectionManager.h"
#include "Communication/Messages.h"
#include "InputSimulator/InputSimulator.h"
#include "Utils/BlockingQueue.h"
#include "Utils/InterruptibleThread.h"

class Processor : public InterruptibleThreadAdapter<
	BlockingQueue<ServerMessage>,
	BlockingQueue<ClientMessage>,
	ConnectionManager,
	InputSimulator> {
protected:
	void onUpdate(BlockingQueue<ServerMessage> &receivedMessages,
		BlockingQueue<ClientMessage> &toSendMessages,
		ConnectionManager &connectionManager,
		InputSimulator &inputSimulator) override;
	ServerMessage messageBuffer;
};
