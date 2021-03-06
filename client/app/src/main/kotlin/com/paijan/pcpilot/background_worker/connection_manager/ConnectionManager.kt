package com.paijan.pcpilot.background_worker.connection_manager

import java.net.InetSocketAddress
import java.util.concurrent.locks.ReentrantReadWriteLock

/**
 * Object managing connection with server over connectionless protocol
 * Receives messages from other modules and processes them on a separate thread
 *
 * In unconnected state it waits for ConnectionRequest message, sends request
 * to specified address and invokes ConnectListener if first KeepAlive
 * is received before timeout
 *
 * In connected state it waits for KeepAlive messages and invokes
 * DisconnectListener in case of a timeout.
 */

typealias ConnectionCallback = (InetSocketAddress) -> Unit

interface ConnectionManager {
    // Thread control
    fun run()
    fun interrupt()
    fun join()
    val lock : ReentrantReadWriteLock

    // State getters
    fun isConnected() : Boolean
    fun getConnectedAddress() : InetSocketAddress

    // Messages enqueues
    fun notifySendConnectionRequest(address: InetSocketAddress)
    fun notifyDisconnect(address: InetSocketAddress)
    fun notifyKeepAlive(address: InetSocketAddress)

    // Callbacks
    val onConnectListener : ConnectionCallback
    val onDisconnectListener : ConnectionCallback
}
