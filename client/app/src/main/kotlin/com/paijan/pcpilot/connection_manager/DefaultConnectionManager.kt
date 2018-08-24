package com.paijan.pcpilot.connection_manager

import android.util.Log
import com.paijan.pcpilot.communication.ServerMessage
import java.net.InetSocketAddress
import java.util.concurrent.BlockingQueue
import java.util.concurrent.LinkedBlockingQueue
import java.util.concurrent.TimeUnit

class DefaultConnectionManager(
        override val onConnectListener: ConnectionCallback,
        override val onDisconnectListener: ConnectionCallback,
        private val toSendMessages: BlockingQueue<ServerMessage>
) : ConnectionManager {
    private val messages: BlockingQueue<in ConnectionManagerMessage> = LinkedBlockingQueue()

    private var thread: Thread? = null
    private var _connectedAddress: InetSocketAddress? = null

    private fun main() {
        messages.clear()

        try {
            var working: Boolean = true
            while (working) {
                if (isConnected()) {
                    working = processMessageConnected()
                } else {
                    working = processMessageUnconnected()
                }
            }
            Log.e("ConnectionManager", "stopped working")
        } catch (e: InterruptedException) {
            Log.i("ConnectionManager", "interrupt")
        }
    }

    private fun processMessageUnconnected(): Boolean {
        val message = messages.take() as? ConnectionManagerSendConnectionRequestMessage
        message ?: return false

        toSendMessages.add(ServerMessage.createMessageConnectionRequest())

        val responseMessage = messages.poll(1000, TimeUnit.MILLISECONDS) // TODO constant
        if (responseMessage != null && (responseMessage is ConnectionManagerKeepAliveMessage) && message.address == responseMessage.address) {
            _connectedAddress = message.address
            onConnectListener(message.address)
        } else {
            onDisconnectListener(message.address)
        }
        return true
    }


    private fun processMessageConnected(): Boolean {
        // TODO not so good implementation
        val message = messages.poll(1000, TimeUnit.MILLISECONDS) as? ConnectionManagerKeepAliveMessage // TODO constant
        if (message == null) {
            onDisconnectListener(_connectedAddress!!)
            _connectedAddress = null
        }
        return true
    }

    override fun run() {
        if (thread != null && thread?.isAlive!!) throw IllegalStateException("Thread already running")
        this.thread = kotlin.concurrent.thread { main() }
    }

    override fun interrupt() {
        thread?.interrupt()
        thread = null
    }

    override fun isConnected(): Boolean = _connectedAddress != null
    override fun getConnectedAddress(): InetSocketAddress = _connectedAddress!!

    override fun sendConnectionRequest(address: InetSocketAddress) {
        messages.add(ConnectionManagerSendConnectionRequestMessage(address))
    }

    override fun notifyKeepAlive(address: InetSocketAddress) {
        messages.add(ConnectionManagerKeepAliveMessage(address))
    }
}
