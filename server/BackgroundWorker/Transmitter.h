#pragma once

#include "Communication/ConnectionlessSocket.h"
#include "Communication/Messages.h"
#include "Utils/InterruptibleThread.h"
#include "Utils/BlockingQueue.h"
#include <vector>

class Transmitter : public InterruptibleThreadAdapter<BlockingQueue<ClientMessage>, ConnectionlessSocket> {
protected:
	void onUpdate(BlockingQueue<ClientMessage>& toSendMessages, ConnectionlessSocket &socket) override;

	std::vector<Byte> byteBuffer;
	std::vector<ClientMessage> messageBuffer;
};
