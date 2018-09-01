package com.paijan.pcpilot.background_worker.connection_manager

import android.util.Log
import com.paijan.pcpilot.utils.Constants
import com.paijan.pcpilot.background_worker.RunnableAdapter
import com.paijan.pcpilot.utils.ServerMessage
import java.net.InetSocketAddress
import java.util.concurrent.BlockingQueue
import java.util.concurrent.LinkedBlockingQueue
import java.util.concurrent.TimeUnit


class DefaultConnectionManager(
        override val onConnectListener: ConnectionCallback,
        override val onDisconnectListener: ConnectionCallback,
        private val toSendMessages: BlockingQueue<ServerMessage>
) : ConnectionManager {
    private val connectionManagerMessages: BlockingQueue<in ConnectionManagerMessage> = LinkedBlockingQueue()

    private var running = false
    private var _connectedAddress: InetSocketAddress? = null
    private var receiverThread: Thread? = null
    private var transmitterThread: Thread? = null


    override fun run() {
        if (running) {
            Log.w("ConnectionManager", "run() called while already running. Ignoring.")
            return
        }

        this.receiverThread = Thread(DefaultConnectionManagerReceiver())
        this.receiverThread?.start()

        this.transmitterThread = Thread(DefaultConnectionManagerTransmitter())
        this.transmitterThread?.start()
    }

    override fun interrupt() {
        if (running) {
            Log.w("ConnectionManager", "interrupt() called while not running. Ignoring.")
            return
        }

        this.receiverThread?.interrupt()
        this.transmitterThread?.interrupt()

    }

    override fun join() {
        this.receiverThread?.interrupt()
        this.transmitterThread?.interrupt()
        this.receiverThread?.join()
        this.transmitterThread?.join()
    }

    override fun isConnected(): Boolean = _connectedAddress != null
    override fun getConnectedAddress(): InetSocketAddress = _connectedAddress!!

    override fun sendConnectionRequest(address: InetSocketAddress) {
        connectionManagerMessages.add(ConnectionManagerSendConnectionRequestMessage(address))
    }

    override fun notifyKeepAlive(address: InetSocketAddress) {
        connectionManagerMessages.add(ConnectionManagerKeepAliveMessage(address))
    }


    inner class DefaultConnectionManagerTransmitter()
        : RunnableAdapter("ConnectionManagerTransmitter") {

        override fun runBody() {
            Thread.sleep(Constants.KEEP_ALIVE_SEND_RATE_MS)

            if (isConnected()) {
                // Possible race condition
                toSendMessages.add(ServerMessage.createMessageKeepAlive(getConnectedAddress()))
            }
        }
    }


    inner class DefaultConnectionManagerReceiver : RunnableAdapter("ConnectionManagerReceiver") {
        override fun runBody() {
            if (isConnected()) processMessageConnected()
            else processMessageUnconnected()
        }

        private fun processMessageUnconnected() {
            val message = connectionManagerMessages.take()
            if (message !is ConnectionManagerSendConnectionRequestMessage) {
                Log.w(messageTag, "Wrong message type when unconnected")
                return
            }

            toSendMessages.add(ServerMessage.createMessageConnectionRequest(message.address))

            val responseMessage = connectionManagerMessages.poll(Constants.KEEP_ALIVE_TIMEOUT_MS, TimeUnit.MILLISECONDS)
            if (responseMessage != null && (responseMessage is ConnectionManagerKeepAliveMessage) && message.address == responseMessage.address) {
                _connectedAddress = message.address
                onConnectListener(message.address)
                Log.i(messageTag, "Connection successful")
            } else {
                onDisconnectListener(message.address)
                Log.i(messageTag, "Connection failed")
            }
        }

        private fun processMessageConnected() {
            // TODO this implementation allows any ConnectionManagerMessage to arrive here
            val message = connectionManagerMessages.poll(Constants.KEEP_ALIVE_TIMEOUT_MS, TimeUnit.MILLISECONDS)
            if (message == null) {
                onDisconnectListener(_connectedAddress!!)
                _connectedAddress = null
                Log.i("ConnectionManager", "Disconnect due to a timeout")
            } else if (message !is ConnectionManagerKeepAliveMessage) {
                Log.w(messageTag, "Wrong message type when connected")
                return
            }
        }
    }


}
