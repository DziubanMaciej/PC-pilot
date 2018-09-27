package com.paijan.pcpilot.background_worker.connection_manager

import android.util.Log
import com.paijan.pcpilot.background_worker.RunnableAdapter
import com.paijan.pcpilot.background_worker.ThreadEndCallback
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
        private val toSendMessages: BlockingQueue<ServerMessage>,
        private val onThreadEnd: ThreadEndCallback
) : ConnectionManager {
    private val connectionManagerMessages: BlockingQueue<ConnectionManagerMessage> = LinkedBlockingQueue()
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

        this.receiverThread = Thread(DefaultConnectionManagerReceiver(onThreadEnd))
        this.receiverThread?.start()

        this.transmitterThread = Thread(DefaultConnectionManagerTransmitter(onThreadEnd))
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


    inner class DefaultConnectionManagerTransmitter(onThreadEnd: ThreadEndCallback)
        : RunnableAdapter("ConnectionManagerTransmitter", onThreadEnd) {

        override fun runBody() {
            Thread.sleep(Constants.KEEP_ALIVE_SEND_RATE_MS)

            lock.read {
                if (isConnected()) {
                    toSendMessages.add(ServerMessage.createMessageKeepAlive(getConnectedAddress()))
                }
            }
        }
    }


    inner class DefaultConnectionManagerReceiver(onThreadEnd: ThreadEndCallback) : RunnableAdapter("ConnectionManagerReceiver", onThreadEnd) {
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

            var shouldConnect = false
            var waitTimeNs = Constants.KEEP_ALIVE_TIMEOUT_MS * 1000000
            lock.read {
                if (isConnected()) {
                    Log.w(messageTag, "Connection acquired while waiting for connection response")
                    return@read
                }

                waitLoop@ while (waitTimeNs > 0) {
                    val timeStart = System.nanoTime()
                    val responseMessage = connectionManagerMessages.poll(Constants.KEEP_ALIVE_TIMEOUT_MS, TimeUnit.MILLISECONDS)
                    waitTimeNs -= System.nanoTime() - timeStart

                    when (responseMessage) {
                        null -> {
                            shouldConnect = false
                            break@waitLoop
                        }
                        is ConnectionManagerKeepAliveMessage -> {
                            val addressMatches = message.address == responseMessage.address
                            shouldConnect = addressMatches
                            break@waitLoop
                        }
                        else -> {
                            shouldConnect = false
                            break@waitLoop
                        }
                    }

                }
            }

            if (shouldConnect) {
                lock.write { connect(message.address) }
            } else {
                Log.i(messageTag, "Failed connecting with ${message.address}")
            }
        }

        private fun processMessageConnected() {
            var shouldDisconnect = true
            var waitTimeNs = Constants.KEEP_ALIVE_TIMEOUT_MS * 1000000
            lock.read {
                if (!isConnected()) {
                    Log.w(messageTag, "Connection lost while waiting for KeepAlive")
                    return@read
                }

                waitLoop@ while (waitTimeNs > 0) {
                    val timeStart = System.nanoTime()
                    val message = connectionManagerMessages.poll(waitTimeNs, TimeUnit.NANOSECONDS)
                    waitTimeNs -= System.nanoTime() - timeStart

                    if (message.address != getConnectedAddress()) {
                        continue@waitLoop
                    }

                    when (message) {
                        null -> {
                            shouldDisconnect = true
                            break@waitLoop
                        }
                        is ConnectionManagerDisconnectRequestMessage -> {
                            shouldDisconnect = true
                            break@waitLoop
                        }
                        is ConnectionManagerKeepAliveMessage -> {
                            shouldDisconnect = false
                            break@waitLoop
                        }
                        else -> {
                            Log.w(messageTag, "Wrong message type when connected, ignoring")
                            shouldDisconnect = true
                            continue@waitLoop
                        }
                    }
                }
            }
            lock.takeIf { shouldDisconnect }?.write { disconnect() }
        }
    }
}
