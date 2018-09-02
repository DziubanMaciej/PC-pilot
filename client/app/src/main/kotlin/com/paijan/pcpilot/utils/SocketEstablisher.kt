package com.paijan.pcpilot.utils

import android.util.Log
import java.net.*


typealias FilterFunction = (InetAddress) -> Boolean

/**
 * Creates 2 sockets on localhost, that can work at the same time
 * If one of sockets could not be created, it increments port number and tries again until success
 */
object SocketEstablisher {

    data class DatagramSocketTuple (
            val receiver: DatagramSocket,
            val sender: DatagramSocket
    )


    fun establishSockets(inetAddress: InetAddress): DatagramSocketTuple? {
        val receiver : DatagramSocket?
        val sender : DatagramSocket?

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
        } catch(e : SocketException) {
            Log.e("SocketEstablisher", "Socket exception")
        }
        return null
    }

    fun closeSockets(sockets: DatagramSocketTuple?) {
        sockets?.receiver?.close()
        sockets?.sender?.close()
    }

    fun getAddresses(filterFunction : FilterFunction? = {true}): MutableList<InetAddress> {
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
        } catch (e: SocketException) { }
        return result
    }

    fun getLocalAddress() : InetAddress? {
        return getAddresses { it.hostAddress.startsWith("192") }[0]
    }
}
