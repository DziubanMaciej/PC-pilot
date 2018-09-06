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
import com.paijan.pcpilot.utils.ServerMessage
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.android.synthetic.main.activity_main.view.*
import java.net.InetAddress
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
        setupTouchPadButtons()
    }

    private fun setupTouchPadButtons() {
        fun sendMessage(createMessage: (InetSocketAddress) -> ServerMessage) {
            connectionManager?.takeIf { it.isConnected() }?.let {
                toSendMessages.add(createMessage(it.getConnectedAddress()))
            }
        }
        root_layout.touchPadButtonLeft.onTouchDown = { sendMessage { ServerMessage.createMessageLeftPress(it) } }
        root_layout.touchPadButtonLeft.onTouchUp = { sendMessage { ServerMessage.createMessageLeftRelease(it) } }
        root_layout.touchPadButtonRight.onTouchDown = { sendMessage { ServerMessage.createMessageRightPress(it) } }
        root_layout.touchPadButtonRight.onTouchUp = { sendMessage { ServerMessage.createMessageRightRelease(it) } }
    }

    override fun onResume() {
        super.onResume()
        updateButtonStates(false)
        setupThreads()
    }

    override fun onPause() {
        super.onPause()

        Log.i("MainActivity", "onResume()")

        // Close sockets first to interrupt all blocking operations
        applicationImpl.sockets?.close()

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

        Log.i("MainActivity", "onResume() ended successfully")
    }

    override fun onBackPressed() {
        finishAffinity()
    }

    private fun setupThreads() {
        connectionManager = DefaultConnectionManager(
                { updateButtonStates(true) },
                { updateButtonStates(false) },
                toSendMessages
        )
        receiver = Thread(Receiver(applicationImpl.sockets?.receiver!!, receivedMessages))
        processor = Thread(Processor(connectionManager!!, receivedMessages, toSendMessages))
        transmitter = Thread(Transmitter(applicationImpl.sockets?.sender!!, toSendMessages))
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
            root_layout.buttonConnect.turn(!connected)
            root_layout.buttonDisconnect.turn(connected)
        }
    }

    @Suppress("UNUSED_PARAMETER")
    fun onClickConnect(v: View?) {
        val socketAddress = InetSocketAddress("192.168.0.60", 9999)
        connectionManager?.notifySendConnectionRequest(socketAddress)
    }

    @Suppress("UNUSED_PARAMETER")
    fun onClickDisconnect(v: View?) {
        connectionManager?.getConnectedAddress()?.let { connectionManager?.notifyDisconnect(it) }
    }
}
