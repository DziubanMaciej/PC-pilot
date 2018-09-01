#include "Processor.h"

void Processor::onUpdate(BlockingQueue<ServerMessage> &receivedMessages, BlockingQueue<ClientMessage> &toSendMessages, ConnectionManager &connectionManager, InputSimulator &inputSimulator) {
	if (!receivedMessages.popAndGet(this->messageBuffer, std::chrono::milliseconds(100))) { // TODO constant
		return;
	}

	switch (this->messageBuffer.getType()) {
		case ServerMessage::Type::ConnectionRequest:
			connectionManager.notifyConnectionRequest(this->messageBuffer.address);
			break;
		case ServerMessage::Type::KeepAlive:
			connectionManager.notifyKeepAlive(this->messageBuffer.address);
			break;
		case ServerMessage::Type::MoveCursor: {
			auto x = this->messageBuffer.getMessageMoveCursorX();
			auto y = this->messageBuffer.getMessageMoveCursorY();
			inputSimulator.moveCursorTo(static_cast<int>(x), static_cast<int>(y));
			break;
		}
		case ServerMessage::Type::LeftPress:
			// TODO
			break;
		case ServerMessage::Type::LeftRelease:
			// TODO
			break;
	}
}
