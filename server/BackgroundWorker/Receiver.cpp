#include "Receiver.h"
#include "Utils/Constants.h"

void Receiver::onUpdate(BlockingQueue<ServerMessage> &receivedMessages, ConnectionlessSocket &socket) {
	if (!socket.receive(this->byteBuffer, this->addressBuffer, Constants::MAX_MESSAGE_SIZE, Constants::MANUAL_LOOP_BACK_RATE_MS)) {
		return;
	}

	ServerMessage::fromBytes(this->byteBuffer, this->addressBuffer, this->messageBuffer);
	for (auto &message : this->messageBuffer) {
		receivedMessages.push(std::move(message));
	}
}
