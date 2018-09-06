package com.paijan.pcpilot.background_worker.connection_manager

import android.util.Log
import com.paijan.pcpilot.background_worker.RunnableAdapter
import com.paijan.pcpilot.utils.Constants
import com.paijan.pcpilot.utils.ServerMessage
import java.net.InetSocketAddress
import java.util.concurrent.BlockingQueue
import java.util.concurrent.LinkedBlockingQueue
import java.util.concurrent.TimeUnit
import java.util.concurrent.locks.ReentrantReadWriteLock
import kotlin.concurrent.read
import kotlin.concurrent.write


class DefaultConnectionManager(
        override val onConnectListener: ConnectionCallback,
        override val onDisconnectListener: ConnectionCallback,
        private val toSendMessages: BlockingQueue<ServerMessage>
) : ConnectionManager {
    private val connectionManagerMessages: BlockingQueue<in ConnectionManagerMessage> = LinkedBlockingQueue()
    private val lock = ReentrantReadWriteLock()

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

    override fun notifySendConnectionRequest(address: InetSocketAddress) {
        connectionManagerMessages.add(ConnectionManagerSendConnectionRequestMessage(address))
    }

    override fun notifyDisconnect(address: InetSocketAddress) {
        connectionManagerMessages.add(ConnectionManagerDisconnectRequestMessage(address))
    }

    override fun notifyKeepAlive(address: InetSocketAddress) {
        connectionManagerMessages.add(ConnectionManagerKeepAliveMessage(address))
    }

    private fun connect(address: InetSocketAddress) {
        _connectedAddress = address
        onConnectListener(address)
        Log.i("ConnectionManager", "Connected with $_connectedAddress")
    }

    private fun disconnect() {
        if (!isConnected()) {
            // Possible race condition
            Log.w("ConnectionManager", "disconnect() called while not connected. Ignoring.")
            return
        }

        Log.i("ConnectionManager", "Disconnected from $_connectedAddress")
        onDisconnectListener(_connectedAddress!!)
        toSendMessages.add(ServerMessage.createMessageDisconnect(_connectedAddress!!))
        _connectedAddress = null
    }


    inner class DefaultConnectionManagerTransmitter
        : RunnableAdapter("ConnectionManagerTransmitter") {

        override fun runBody() {
            Thread.sleep(Constants.KEEP_ALIVE_SEND_RATE_MS)

            lock.read {
                if (isConnected()) {
                    toSendMessages.add(ServerMessage.createMessageKeepAlive(getConnectedAddress()))
                }
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

            var shouldConnect = false
            lock.read {
                if (isConnected()) {
                    Log.w(messageTag, "Connection acquired while waiting for connection response")
                    return@read
                }

                if (responseMessage != null && (responseMessage is ConnectionManagerKeepAliveMessage) && message.address == responseMessage.address) {
                    shouldConnect = true
                } else {
                    Log.i(messageTag, "Failed connecting with ${message.address}")
                }
            }
            lock.takeIf { shouldConnect }?.write { connect(message.address) }
        }

        private fun processMessageConnected() {
            // TODO this implementation allows any ConnectionManagerMessage to arrive here
            val message = connectionManagerMessages.poll(Constants.KEEP_ALIVE_TIMEOUT_MS, TimeUnit.MILLISECONDS)

            var shouldDisconnect = false
            lock.read {
                if (!isConnected()) {
                    Log.w(messageTag, "Connection lost while waiting for KeepAlive")
                    return@read
                }

                if (message == null || message is ConnectionManagerDisconnectRequestMessage) {
                    shouldDisconnect = true
                } else if (message !is ConnectionManagerKeepAliveMessage) {
                    Log.w(messageTag, "Wrong message type when connected")
                }
            }
            lock.takeIf { shouldDisconnect }?.write { disconnect() }
        }
    }
}
