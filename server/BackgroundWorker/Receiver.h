#pragma once

#include "Communication/ConnectionlessSocket.h"
#include "Communication/Messages.h"
#include "Utils/InterruptibleThread.h"
#include "Utils/BlockingQueue.h"
#include <vector>

class Receiver : public InterruptibleThreadAdapter<BlockingQueue<ServerMessage>, ConnectionlessSocket> {
protected:
	void onUpdate(BlockingQueue<ServerMessage>& receivedMessages, ConnectionlessSocket &socket) override;

	InetAddress addressBuffer;
	std::vector<Byte> byteBuffer;
	std::vector<ServerMessage> messageBuffer;
};
