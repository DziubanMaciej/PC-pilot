package com.paijan.pcpilot.background_worker

import com.paijan.pcpilot.utils.Constants
import com.paijan.pcpilot.utils.ServerMessage
import java.net.DatagramPacket
import java.net.DatagramSocket
import java.net.SocketException
import java.util.concurrent.BlockingQueue

class Transmitter(
        private val socket: DatagramSocket,
        private val toSendMessages: BlockingQueue<ServerMessage>,
        onThreadEnd: ThreadEndCallback
) : RunnableAdapter("Transmitter", onThreadEnd) {

    @Throws(InterruptedException::class, IllegalStateException::class)
    override fun runBody() {
        val messages = mutableListOf<ServerMessage>()
        messages.add(toSendMessages.take()) // block until first message
        while (hasSpaceForAnotherMessage(messages)) {
            val message = toSendMessages.poll() ?: break // non blocking call
            messages.add(message)
        }

        // TODO code below assumes all messages have the same destination address
        val data = ServerMessage.toBytes(messages)
        val packet = DatagramPacket(data, data.size, messages[0].address)
        try {
            socket.send(packet)
        } catch (e: SocketException) {
            throw InterruptedException()
        }
    }

    private fun hasSpaceForAnotherMessage(messages: List<ServerMessage>): Boolean {
        val sizeAfterAdding = ServerMessage.SIZE * (messages.size + 1)
        return sizeAfterAdding <= Constants.MAX_MESSAGE_SIZE
    }
}
