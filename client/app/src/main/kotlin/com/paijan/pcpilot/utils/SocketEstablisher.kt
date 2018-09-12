package com.paijan.pcpilot.utils

import android.util.Log
import java.net.*


/**
 * A runnable that repeatedly tries to bind socket to local ip address on an ephemeral port
 * When it succeeds it calls a callback provided in constructor
 */

data class DatagramSocketTuple(
        val broadcastReceiver: DatagramSocket,
        val receiver: DatagramSocket,
        val sender: DatagramSocket
) : AutoCloseable {
    override fun close() {
        broadcastReceiver.close()
        receiver.close()
        sender.close()
    }
}

class SocketEstablisher(val callback: (DatagramSocketTuple) -> Unit) : Runnable {
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

    private fun establishSockets(inetAddress: InetAddress): DatagramSocketTuple? {
        val broadcastReceiver: DatagramSocket?
        val receiver: DatagramSocket?
        val sender: DatagramSocket?

        try {
            broadcastReceiver = DatagramSocket(null)
            val broadcastReceiverSocketAddress = InetSocketAddress(Constants.AVAILABLE_CLIENT_PORTS[0])
            broadcastReceiver.reuseAddress = true
            broadcastReceiver.bind(broadcastReceiverSocketAddress)

            sender = DatagramSocket(null)
            val senderSocketAddress = InetSocketAddress(inetAddress, Constants.AVAILABLE_CLIENT_PORTS[0])
            sender.reuseAddress = true
            sender.bind(senderSocketAddress)

            receiver = DatagramSocket(null)
            val receiverSocketAddress = InetSocketAddress(inetAddress, Constants.AVAILABLE_CLIENT_PORTS[0])
            receiver.reuseAddress = true
            receiver.bind(receiverSocketAddress)

            Log.i("SocketEstablisher", "Sockets established on $senderSocketAddress")
            return DatagramSocketTuple(broadcastReceiver, receiver, sender)
        } catch (e: SocketException) {
            Log.e("SocketEstablisher", "Socket exception")
        }
        return null
    }

    private fun getAddresses(filterFunction: ((InetAddress) -> Boolean)? = { true }): MutableList<InetAddress> {
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

    private fun getLocalAddress(): InetAddress? {
        return getAddresses { it.hostAddress.startsWith("192") }.getOrNull(0)
    }
}
