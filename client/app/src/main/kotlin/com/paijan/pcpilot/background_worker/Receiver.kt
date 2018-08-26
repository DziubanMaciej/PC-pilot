package com.paijan.pcpilot.background_worker

import com.paijan.pcpilot.communication.ClientMessage
import java.net.DatagramPacket
import java.net.DatagramSocket
import java.net.InetSocketAddress
import java.util.concurrent.BlockingQueue

class Receiver(
        private val socket: DatagramSocket,
        private val receivedMessages: BlockingQueue<ClientMessage>,
        bufferSize: Int = 512
) : RunnableAdapter("Receiver") {
    private val packet: DatagramPacket = DatagramPacket(ByteArray(bufferSize), bufferSize)

    @Throws(InterruptedException::class, IllegalStateException::class)
    override fun runBody() {
        if (Thread.interrupted()) throw InterruptedException()

        socket.receive(packet)
        for (message in ClientMessage.fromBytes(packet.data, packet.length, packet.socketAddress as InetSocketAddress)) {
            receivedMessages.add(message) // may throw
        }
    }
}