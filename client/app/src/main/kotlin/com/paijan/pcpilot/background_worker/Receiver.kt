package com.paijan.pcpilot.background_worker

import com.paijan.pcpilot.utils.ClientMessage
import com.paijan.pcpilot.utils.Constants
import java.net.DatagramPacket
import java.net.DatagramSocket
import java.net.InetSocketAddress
import java.net.SocketException
import java.util.concurrent.BlockingQueue

class Receiver(
        private val socket: DatagramSocket,
        private val receivedMessages: BlockingQueue<ClientMessage>,
        onThreadEnd: ThreadEndCallback
) : RunnableAdapter("Receiver", onThreadEnd) {
    private val packet: DatagramPacket = DatagramPacket(ByteArray(Constants.MAX_MESSAGE_SIZE), Constants.MAX_MESSAGE_SIZE)

    @Throws(InterruptedException::class)
    override fun runBody() {
        try {
            socket.receive(packet)
        } catch (e: SocketException) {
            throw InterruptedException()
        }

        for (message in ClientMessage.fromBytes(packet.data, packet.length, packet.socketAddress as InetSocketAddress)) {
            receivedMessages.add(message) // may throw
        }
    }
}