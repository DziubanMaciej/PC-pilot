package com.paijan.pcpilot.activity

import android.app.Activity
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.Button
import com.paijan.pcpilot.R
import com.paijan.pcpilot.background_worker.Processor
import com.paijan.pcpilot.background_worker.Receiver
import com.paijan.pcpilot.background_worker.Transmitter
import com.paijan.pcpilot.background_worker.connection_manager.ConnectionManager
import com.paijan.pcpilot.background_worker.connection_manager.DefaultConnectionManager
import com.paijan.pcpilot.utils.ClientMessage
import com.paijan.pcpilot.utils.DatagramSocketTuple
import com.paijan.pcpilot.utils.ServerMessage
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.android.synthetic.main.activity_main.view.*
import java.net.InetSocketAddress
import java.util.concurrent.LinkedBlockingQueue

class MainActivity : Activity() {
    private val receivedMessages = LinkedBlockingQueue<ClientMessage>()
    private val toSendMessages = LinkedBlockingQueue<ServerMessage>()

    private val applicationImpl get() = (application as ApplicationImpl)
    private var connectionManager: ConnectionManager? = null
    private var receiver: Thread? = null
    private var processor: Thread? = null
    private var transmitter: Thread? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        resetThreads()
        setupThreads()
    }

    override fun onDestroy() {
        resetThreads()
        super.onDestroy()
    }

    private fun resetThreads() {
        Log.i("MainActivity", "resetThreads called()")
        updateButtonStates(false)

        receiver?.interrupt()
        processor?.interrupt()
        transmitter?.interrupt()
        connectionManager?.interrupt()

        receiver?.join()
        processor?.join()
        transmitter?.join()
        connectionManager?.join()

        receiver = null
        processor = null
        transmitter = null
        connectionManager = null

        receivedMessages.clear()
        toSendMessages.clear()

        applicationImpl.sockets?.close()
    }

    private fun setupThreads() {
        connectionManager = DefaultConnectionManager(
                { updateButtonStates(true) },
                { updateButtonStates(false) },
                toSendMessages
        )
        receiver = Thread(Receiver(applicationImpl.sockets?.receiver!!, receivedMessages))
        processor = Thread(Processor(connectionManager!!, receivedMessages, toSendMessages))
        transmitter = Thread(Transmitter(applicationImpl.sockets?.receiver!!, toSendMessages))
        root_layout.touchPad.onSendCursorMoveCallback = { x, y ->
            connectionManager?.takeIf { it.isConnected() }?.let {
                toSendMessages.add(ServerMessage.createMessageMoveCursor(it.getConnectedAddress(), x, y))
            }
        }

        connectionManager?.run()
        receiver?.start()
        processor?.start()
        transmitter?.start()
    }

    private fun updateButtonStates(connected: Boolean) {
        fun Button.turn(on: Boolean) {
            isClickable = on
            alpha = if (on) 1.0f else 0.3f
        }

        runOnUiThread {
            root_layout.button_connect.turn(!connected)
            root_layout.button_disconnect.turn(connected)
        }
    }

    @Suppress("UNUSED_PARAMETER")
    fun onClickConnect(v: View?) {
        val socketAddress = InetSocketAddress("192.168.0.60", 9999)
        connectionManager?.sendConnectionRequest(socketAddress)
    }

    @Suppress("UNUSED_PARAMETER")
    fun onClickDisconnect(v: View?) {
        connectionManager?.disconnect()
    }
}
