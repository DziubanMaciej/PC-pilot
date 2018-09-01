package com.paijan.pcpilot.activity

import android.app.Activity
import android.os.Bundle
import android.view.View
import com.paijan.pcpilot.R
import com.paijan.pcpilot.utils.SocketEstablisher
import com.paijan.pcpilot.background_worker.Processor
import com.paijan.pcpilot.background_worker.Receiver
import com.paijan.pcpilot.background_worker.Transmitter
import com.paijan.pcpilot.utils.ClientMessage
import com.paijan.pcpilot.utils.ServerMessage
import com.paijan.pcpilot.background_worker.connection_manager.ConnectionManager
import com.paijan.pcpilot.background_worker.connection_manager.DefaultConnectionManager
import java.net.InetSocketAddress
import java.util.concurrent.LinkedBlockingQueue

class MainActivity : Activity() {
    var connectionManager : ConnectionManager? = null
    var receiver : Thread? = null
    var processor : Thread? = null
    var transmitter : Thread? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
    }

    @Suppress("UNUSED_PARAMETER")
    fun onCreateSocket(v: View?) {
        val address = SocketEstablisher.getLocalAddresses { it.hostAddress.startsWith("192") }[0]
        val sockets = SocketEstablisher.establishSockets(address)!!

        val receivedMessages = LinkedBlockingQueue<ClientMessage>()
        val toSendMessages = LinkedBlockingQueue<ServerMessage>()

        connectionManager = DefaultConnectionManager({ }, {}, toSendMessages)
        receiver = Thread(Receiver(sockets.receiver, receivedMessages))
        processor = Thread(Processor(connectionManager!!, receivedMessages, toSendMessages))
        transmitter = Thread(Transmitter(sockets.receiver, toSendMessages))

        connectionManager?.run()
        receiver?.start()
        processor?.start()
        transmitter?.start()
    }

    @Suppress("UNUSED_PARAMETER")
    fun onDestroySocket(v: View?) {
        connectionManager?.interrupt()
        connectionManager = null

        receiver?.interrupt()
        receiver = null

        processor?.interrupt()
        processor = null

        transmitter?.interrupt()
        transmitter = null
    }

    @Suppress("UNUSED_PARAMETER")
    fun onSendConnectionRequest(v: View?) {
        val socketAddress = InetSocketAddress("192.168.0.60", 9999);
        connectionManager?.sendConnectionRequest(socketAddress)
    }
}
