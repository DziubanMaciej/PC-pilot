package com.paijan.pcpilot.background_worker

import com.paijan.pcpilot.Constants
import com.paijan.pcpilot.communication.ServerMessage
import java.net.DatagramPacket
import java.net.DatagramSocket
import java.util.concurrent.BlockingQueue

class Transmitter(
        private val socket: DatagramSocket,
        private val toSendMessages: BlockingQueue<ServerMessage>
) : RunnableAdapter("Transmitter") {

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
        socket.send(packet)
    }

    private fun hasSpaceForAnotherMessage(messages : List<ServerMessage>) : Boolean {
        val sizeAfterAdding = ServerMessage.SIZE * (messages.size + 1)
        return sizeAfterAdding <= Constants.MAX_MESSAGE_SIZE
    }
}
