#include "Transmitter.h"
#include "Utils/Constants.h"

void Transmitter::onUpdate(BlockingQueue<ClientMessage>& toSendMessages, ConnectionlessSocket & socket) {
	// No trying to send multiple messages at once, due to only KeepAlive being sent

	this->messageBuffer.resize(1);
	if (!toSendMessages.popAndGet(this->messageBuffer[0], std::chrono::milliseconds(Constants::MANUAL_LOOP_BACK_RATE_MS))) {
		return;
	}

	ClientMessage::toBytes(this->messageBuffer, this->byteBuffer);
	socket.send(this->byteBuffer, this->messageBuffer[0].address);
}
