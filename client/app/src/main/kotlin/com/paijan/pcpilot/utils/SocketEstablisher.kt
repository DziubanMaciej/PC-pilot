package com.paijan.pcpilot.utils

import android.util.Log
import java.net.*


typealias FilterFunction = (InetAddress) -> Boolean

/**
 * A runnable that repeatedly tries to bind socket to local ip address on an ephemeral port
 * When it succeeds it calls a callback provided in constructor
 */
class SocketEstablisher(val callback: (SocketEstablisher.DatagramSocketTuple) -> Unit) : Runnable {
    data class DatagramSocketTuple(
            val receiver: DatagramSocket,
            val sender: DatagramSocket
    ) : AutoCloseable {
        override fun close() {
            receiver.close()
            sender.close()
        }
    }

    override fun run() {
        while (true) {
            val sockets = getLocalAddress()?.let { establishSockets(it) }
            if (sockets == null) {
                Thread.sleep(Constants.MANUAL_LOOP_BACK_RATE_MS)
                continue
            }

            callback(sockets)
            break
        }
    }

    fun establishSockets(inetAddress: InetAddress): DatagramSocketTuple? {
        val receiver: DatagramSocket?
        val sender: DatagramSocket?

        try {
            receiver = DatagramSocket(null)
            val receiverSocketAddress = InetSocketAddress(inetAddress, 0)
            receiver.reuseAddress = true
            receiver.bind(receiverSocketAddress)

            sender = DatagramSocket(null)
            val senderSocketAddress = InetSocketAddress(inetAddress, receiver.localPort)
            sender.reuseAddress = true
            sender.bind(senderSocketAddress)

            return DatagramSocketTuple(sender, receiver)
        } catch (e: SocketException) {
            Log.e("SocketEstablisher", "Socket exception")
        }
        return null
    }

    fun getAddresses(filterFunction: FilterFunction? = { true }): MutableList<InetAddress> {
        val result: MutableList<InetAddress> = mutableListOf()
        try {
            val networkInterfaces = NetworkInterface.getNetworkInterfaces()
            for (networkInterface in networkInterfaces) {
                for (address in networkInterface.inetAddresses) {
                    if (filterFunction == null || filterFunction(address)) {
                        result.add(address)
                    }
                }
            }
        } catch (e: SocketException) {
        }
        return result
    }

    fun getLocalAddress(): InetAddress? {
        return getAddresses { it.hostAddress.startsWith("192") }[0]
    }
}
