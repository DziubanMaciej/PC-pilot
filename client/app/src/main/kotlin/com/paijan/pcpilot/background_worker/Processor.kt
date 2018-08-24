package com.paijan.pcpilot.background_worker

import android.util.Log
import com.paijan.pcpilot.communication.ClientMessage
import com.paijan.pcpilot.communication.ServerMessage
import java.util.concurrent.BlockingQueue

class Processor(
        private val receivedMessages: BlockingQueue<ServerMessage>,
        private val toSendMessages: BlockingQueue<ClientMessage>
) : RunnableAdapter("Processor") {

    @Throws(InterruptedException::class)
    override fun runBody() {
        val message = receivedMessages.take() // blocks
        when (message.getType()) {

            else -> {
                Log.w("Processor", "Unknown message, ignoring")
            }
        }
    }
}

