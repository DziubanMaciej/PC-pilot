package com.paijan.pcpilot.background_worker

import android.util.Log
import com.paijan.pcpilot.utils.ClientMessage
import com.paijan.pcpilot.utils.ServerMessage
import com.paijan.pcpilot.background_worker.connection_manager.ConnectionManager
import java.util.concurrent.BlockingQueue

class Processor(
        private val connectionManager: ConnectionManager,
        private val receivedMessages: BlockingQueue<ClientMessage>,
        private val toSendMessages: BlockingQueue<ServerMessage>
) : RunnableAdapter("Processor") {

    @Throws(InterruptedException::class)
    override fun runBody() {
        val message = receivedMessages.take() // blocks

        when (message.getType()) {
            ClientMessage.Type.KeepAlive -> {
                // TODO validate client IP
                connectionManager.notifyKeepAlive(message.address)
            }
            else -> {
                Log.w("Processor", "Unknown message, ignoring")
            }
        }
    }
}

