package com.paijan.pcpilot

import java.net.*
import java.util.Collections
import java.util.Enumeration
import java.util.function.Predicate
import java.util.stream.Stream


interface FilterFunction {
    fun filter(address : InetAddress) : Boolean
}

/**
 * Creates 2 sockets on localhost, that can work at the same time
 * If one of sockets could not be created, it increments port number and tries again until success
 */
object SocketEstablisher {

    fun getLocalAddresses(filterFunction : FilterFunction?): MutableList<InetAddress> {
        val result: MutableList<InetAddress> = mutableListOf()
        try {
            val networkInterfaces = NetworkInterface.getNetworkInterfaces()
            for (networkInterface in networkInterfaces) {
                for (address in networkInterface.inetAddresses) {
                    if (filterFunction == null || filterFunction.filter(address)) {
                        result.add(address)
                    }
                }
            }
        } catch (e: SocketException) { }
        return result
    }

    private fun isPortAvailable(port: Int, localAddress: InetAddress): Boolean {
        try {
            DatagramSocket(port, localAddress).use { ignored -> return true }
        } catch (e: SocketException) {
            return false
        }

    }
}
