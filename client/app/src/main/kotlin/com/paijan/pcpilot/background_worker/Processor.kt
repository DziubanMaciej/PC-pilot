package com.paijan.pcpilot.background_worker

import android.util.Log
import com.paijan.pcpilot.background_worker.connection_manager.ConnectionManager
import com.paijan.pcpilot.utils.ClientMessage
import java.net.InetSocketAddress
import java.util.concurrent.BlockingQueue

class Processor(
        private val connectionManager: ConnectionManager,
        private val receivedMessages: BlockingQueue<ClientMessage>,
        private val onAdvertiseCallback: (InetSocketAddress) -> Unit,
        onThreadEnd: ThreadEndCallback
) : RunnableAdapter("Processor", onThreadEnd) {

    @Throws(InterruptedException::class)
    override fun runBody() {
        val message = receivedMessages.take() // blocks

        when (message.getType()) {
            ClientMessage.Type.Advertise -> {
                onAdvertiseCallback(message.address)
            }
            ClientMessage.Type.KeepAlive -> {
                // TODO validate client IP
                connectionManager.notifyKeepAlive(message.address)
            }
            ClientMessage.Type.Disconnect -> {
                // TODO validate client IP
                connectionManager.notifyDisconnect(message.address)
            }
            else -> {
                Log.w(messageTag, "Unknown message, ignoring")
            }
        }
    }
}

