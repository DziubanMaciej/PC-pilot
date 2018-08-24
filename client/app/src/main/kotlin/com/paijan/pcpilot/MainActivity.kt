package com.paijan.pcpilot

import java.net.Socket

import android.app.Activity
import android.os.Bundle
import android.view.View
import com.paijan.pcpilot.background_worker.Processor
import com.paijan.pcpilot.background_worker.Receiver
import com.paijan.pcpilot.background_worker.Transmitter
import com.paijan.pcpilot.communication.ClientMessage
import com.paijan.pcpilot.communication.ServerMessage
import com.paijan.pcpilot.connection_manager.ConnectionManager
import com.paijan.pcpilot.connection_manager.DefaultConnectionManager
import java.net.InetAddress
import java.net.InetSocketAddress
import java.util.concurrent.LinkedBlockingQueue

class MainActivity : Activity() {
    private val socket: Socket? = null

    var connectionManager : ConnectionManager? = null
    var receiver : Thread? = null
    var processor : Thread? = null
    var transmitter : Thread? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
    }

    fun onClick1(v: View?) {
        val address = SocketEstablisher.getLocalAddresses { it.hostAddress.startsWith("192")}[0]
        val sockets = SocketEstablisher.establishSockets(address)!!

        val receivedMessages = LinkedBlockingQueue<ClientMessage>()
        val toSendMessages = LinkedBlockingQueue<ServerMessage>()

        connectionManager = DefaultConnectionManager({ }, {}, toSendMessages)
        receiver = Thread(Receiver(sockets.receiver, receivedMessages))
        processor = Thread(Processor(receivedMessages, toSendMessages))
        transmitter = Thread(Transmitter(sockets.receiver, toSendMessages))

        connectionManager?.run()
        receiver?.start()
        processor?.start()
        transmitter?.start()
    }

    fun onClick2(v: View?) {
        connectionManager?.interrupt()
        connectionManager = null

        receiver?.interrupt()
        receiver = null

        processor?.interrupt()
        processor = null

        transmitter?.interrupt()
        transmitter = null
    }

    fun onClick3(v: View?) {

    }
}
