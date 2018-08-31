#include "Transmitter.h"

void Transmitter::onUpdate(BlockingQueue<ClientMessage>& toSendMessages, ConnectionlessSocket & socket) {
	// No trying to send multiple messages at once, due to only KeepAlive being sent

	this->messageBuffer.resize(1);
	if (!toSendMessages.popAndGet(this->messageBuffer[0], std::chrono::milliseconds(100))) {
		return;
	}

	ClientMessage::toBytes(this->messageBuffer, this->byteBuffer);
	socket.send(this->byteBuffer, this->messageBuffer[0].address);
}
