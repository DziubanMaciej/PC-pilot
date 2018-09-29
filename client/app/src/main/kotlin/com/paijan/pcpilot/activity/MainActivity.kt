package com.paijan.pcpilot.activity

import android.app.Activity
import android.os.Bundle
import android.support.v7.widget.DividerItemDecoration
import android.support.v7.widget.LinearLayoutManager
import android.view.KeyEvent
import android.view.View
import com.paijan.pcpilot.R
import com.paijan.pcpilot.background_worker.Processor
import com.paijan.pcpilot.background_worker.Receiver
import com.paijan.pcpilot.background_worker.ServerListClearer
import com.paijan.pcpilot.background_worker.Transmitter
import com.paijan.pcpilot.background_worker.connection_manager.ConnectionManager
import com.paijan.pcpilot.background_worker.connection_manager.DefaultConnectionManager
import com.paijan.pcpilot.utils.*
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.android.synthetic.main.activity_main.view.*
import java.util.concurrent.LinkedBlockingQueue

class MainActivity : Activity() {
    private val activityEnder = ActivityEnder(this)
    private val receivedMessages = LinkedBlockingQueue<ClientMessage>()
    private val toSendMessages = LinkedBlockingQueue<ServerMessage>()

    private val keyboard = Keyboard(this)
    private val applicationImpl get() = (application as ApplicationImpl)
    private lateinit var serverRecyclerViewAdapter: ServerRecyclerViewAdapter

    private var connectionManager: ConnectionManager? = null
    private var receiver: Thread? = null
    private var broadcastReceiver: Thread? = null
    private var processor: Thread? = null
    private var transmitter: Thread? = null
    private var serverListClearer: Thread? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        setupTouchPad()
        setupServerList()
    }

    override fun onResume() {
        super.onResume()
        setupThreads()
    }

    private fun setupTouchPad() {
        root.apply {
            touchPadButtonLeft.onTouchDown = { MessageSendTask.createAndExecute({ ServerMessage.createMessageLeftPress(it) }, connectionManager, toSendMessages) }
            touchPadButtonLeft.onTouchUp = { MessageSendTask.createAndExecute({ ServerMessage.createMessageLeftRelease(it) }, connectionManager, toSendMessages) }
            touchPadButtonRight.onTouchDown = { MessageSendTask.createAndExecute({ ServerMessage.createMessageRightPress(it) }, connectionManager, toSendMessages) }
            touchPadButtonRight.onTouchUp = { MessageSendTask.createAndExecute({ ServerMessage.createMessageRightRelease(it) }, connectionManager, toSendMessages) }
            touchPad.onSendCursorMoveCallback = { x, y -> MessageSendTask.createAndExecute({ ServerMessage.createMessageMoveCursor(it, x, y) }, connectionManager, toSendMessages) }
        }
    }

    private fun setupServerList() {
        serverRecyclerViewAdapter = ServerRecyclerViewAdapter(
                root.serverListWrapper,
                { connectionManager!!.isConnected() },
                { connectionManager?.notifySendConnectionRequest(it) }
        )
        root.serverList.apply {
            val linearLayoutManager = LinearLayoutManager(this.context)
            this.layoutManager = linearLayoutManager
            this.adapter = serverRecyclerViewAdapter
            this.addItemDecoration(DividerItemDecoration(this.context, linearLayoutManager.orientation))
        }
    }

    private fun setupThreads() {
        connectionManager = DefaultConnectionManager(
                { this@MainActivity.runOnUiThread { serverRecyclerViewAdapter.onDataSetChanged() } },
                { this@MainActivity.runOnUiThread { serverRecyclerViewAdapter.onDataSetChanged() } },
                toSendMessages,
                activityEnder.onThreadEndCallback
        ).apply { run() }

        receiver = Thread(Receiver(
                applicationImpl.sockets?.receiver!!,
                receivedMessages,
                activityEnder.onThreadEndCallback
        )).apply { start() }


        broadcastReceiver = Thread(Receiver(
                applicationImpl.sockets?.broadcastReceiver!!,
                receivedMessages,
                activityEnder.onThreadEndCallback
        )).apply { start() }

        processor = Thread(Processor(
                connectionManager!!,
                receivedMessages,
                { runOnUiThread { serverRecyclerViewAdapter.addEntry(it) } },
                activityEnder.onThreadEndCallback
        )).apply { start() }

        transmitter = Thread(Transmitter(
                applicationImpl.sockets?.sender!!,
                toSendMessages,
                activityEnder.onThreadEndCallback
        )).apply { start() }

        serverListClearer = Thread(ServerListClearer(
                serverRecyclerViewAdapter,
                activityEnder.onThreadEndCallback
        )).apply { start() }
    }


    override fun onPause() {
        super.onPause()

        // Close sockets first to interrupt all blocking operations
        applicationImpl.sockets?.close()

        receiver?.interrupt()
        broadcastReceiver?.interrupt()
        processor?.interrupt()
        transmitter?.interrupt()
        connectionManager?.interrupt()
        serverListClearer?.interrupt()

        receiver?.join()
        broadcastReceiver?.join()
        processor?.join()
        transmitter?.join()
        connectionManager?.join()
        serverListClearer?.join()

        receiver = null
        broadcastReceiver = null
        processor = null
        transmitter = null
        connectionManager = null
        serverListClearer = null

        receivedMessages.clear()
        toSendMessages.clear()
    }

    override fun onBackPressed() {
        activityEnder.endApplication()
    }

    @Suppress("UNUSED_PARAMETER")
    fun onClickDisconnect(v: View?) {
        connectionManager!!.apply {
            notifyDisconnect(getConnectedAddress())
        }
    }

    @Suppress("UNUSED_PARAMETER")
    fun onKeyboardToggle(v: View?) {
        keyboard.toggleShow()
    }

    override fun onKeyDown(keyCode: Int, event: KeyEvent?): Boolean {
        return keyboard.onKeyDown(event, connectionManager, toSendMessages) || super.onKeyDown(keyCode, event)
    }
}
