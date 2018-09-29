#include "Processor.h"
#include "Utils/Constants.h"

void Processor::onUpdate(BlockingQueue<ServerMessage> &receivedMessages, BlockingQueue<ClientMessage> &toSendMessages, ConnectionManager &connectionManager, InputSimulator &inputSimulator) {
	if (!receivedMessages.popAndGet(this->messageBuffer, std::chrono::milliseconds(Constants::MANUAL_LOOP_BACK_RATE_MS))) {
		return;
	}

	switch (this->messageBuffer.getType()) {
		case ServerMessage::Type::ConnectionRequest:
			connectionManager.notifyConnectionRequest(this->messageBuffer.address);
			break;
		case ServerMessage::Type::Disconnect:
			connectionManager.notifyDisconnect(this->messageBuffer.address);
			break;
		case ServerMessage::Type::KeepAlive:
			connectionManager.notifyKeepAlive(this->messageBuffer.address);
			break;
		case ServerMessage::Type::MoveCursor: {
			auto x = this->messageBuffer.getMessageMoveCursorX();
			auto y = this->messageBuffer.getMessageMoveCursorY();
			inputSimulator.moveCursorBy(static_cast<int>(x), static_cast<int>(y));
			break;
		}
		case ServerMessage::Type::LeftPress:
			inputSimulator.sendMouseButtonEvent(InputSimulator::MouseButtonEventType::LeftPress);
			break;
		case ServerMessage::Type::LeftRelease:
			inputSimulator.sendMouseButtonEvent(InputSimulator::MouseButtonEventType::LeftRelease);
			break;
		case ServerMessage::Type::RightPress:
			inputSimulator.sendMouseButtonEvent(InputSimulator::MouseButtonEventType::RightPress);
			break;
		case ServerMessage::Type::RightRelease:
			inputSimulator.sendMouseButtonEvent(InputSimulator::MouseButtonEventType::RightRelease);
			break;
		case ServerMessage::Type::KeyPress:
			auto unicode = this->messageBuffer.getMessageKeyPressUnicode();
			inputSimulator.pressButtonUnicode(unicode);
			break;
	}
}
