package com.paijan.pcpilot.background_worker

import android.util.Log
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
        val maxMessageSize = 128 // TODO extract to some shared constant

        val messages = mutableListOf<ServerMessage>()
        messages.add(toSendMessages.take()) // block until first message
        while (ServerMessage.SIZE * (messages.size + 1) < maxMessageSize) {
            val message = toSendMessages.poll() ?: break // non blocking call
            messages.add(message)
        }

        val data = ServerMessage.toBytes(messages)
        val packet = DatagramPacket(data, data.size)
        // TODO actually send
        // packet.address =
        // packet.port =
        // socket.send(packet)
    }
}