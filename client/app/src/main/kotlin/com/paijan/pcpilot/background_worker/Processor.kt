package com.paijan.pcpilot.background_worker

import android.util.Log
import com.paijan.pcpilot.communication.ClientMessage
import com.paijan.pcpilot.communication.ServerMessage
import java.util.concurrent.BlockingQueue

class Processor(
        private val receivedMessages: BlockingQueue<ServerMessage>,
        private val toSendMessages: BlockingQueue<ClientMessage>
) : Runnable {

    override fun run() {
        Log.i("Processor", "start")
        try {
            while (true) {
                runBody()
            }
        } catch (e: InterruptedException) {
            Log.e("Processor", "Thread interrupted")
        }
        Log.i("Processor", "end")
    }

    @Throws(InterruptedException::class, IllegalStateException::class)
    private fun runBody() {
        val message = receivedMessages.take() // blocks
        when (message.getType()) {
            ClientMessage.Type.Connect -> {

            }
            else -> {
                Log.w("Processor", "Unknown message, ignoring")
            }
        }
    }
}

