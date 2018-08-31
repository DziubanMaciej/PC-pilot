#include "Receiver.h"
#include "Utils/Constants.h"

void Receiver::onUpdate(BlockingQueue<ServerMessage> &receivedMessages, ConnectionlessSocket &socket) {
	// TODO constant
	if (!socket.receive(this->byteBuffer, this->addressBuffer, Constants::MAX_MESSAGE_SIZE, 100)) {
		return;
	}

	ServerMessage::fromBytes(this->byteBuffer, this->addressBuffer, this->messageBuffer);
	for (auto &message : this->messageBuffer) {
		receivedMessages.push(std::move(message));
	}
}
