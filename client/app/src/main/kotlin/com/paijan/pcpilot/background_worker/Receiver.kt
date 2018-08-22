package com.paijan.pcpilot.background_worker

import android.util.Log
import com.paijan.pcpilot.communication.ClientMessage
import java.net.DatagramPacket
import java.net.DatagramSocket
import java.util.concurrent.BlockingQueue

class Receiver(
        private val socket: DatagramSocket,
        private val receivedMessages: BlockingQueue<ClientMessage>,
        bufferSize: Int = 512
) : Runnable {
    private val packet: DatagramPacket = DatagramPacket(ByteArray(bufferSize), bufferSize)

    override fun run() {
        Log.i("Receiver", "start")
        try {
            while (true) {
                runBody()
            }
        } catch (e: InterruptedException) {
            Log.e("Receiver", "Thread interrupted")
        }
        Log.i("Receiver", "end")
    }

    @Throws(InterruptedException::class, IllegalStateException::class)
    private fun runBody() {
        if (Thread.interrupted()) throw InterruptedException()

        socket.receive(packet) // TODO validate client IP
        for (message in ClientMessage.fromBytes(packet.data, packet.length)) {
            receivedMessages.add(message) // may throw
        }
    }
}