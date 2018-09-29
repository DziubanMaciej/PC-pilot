package com.paijan.pcpilot.utils

import android.os.AsyncTask
import com.paijan.pcpilot.background_worker.connection_manager.ConnectionManager
import java.net.InetSocketAddress
import java.util.concurrent.BlockingQueue
import kotlin.concurrent.read

typealias MessageCreateFunction = (InetSocketAddress) -> ServerMessage

data class MessageSendTaskArgs(
        val createMessage: MessageCreateFunction,
        val connectionManager: ConnectionManager?,
        val toSendMessages: BlockingQueue<ServerMessage>
)

class MessageSendTask : AsyncTask<MessageSendTaskArgs, Void, Void>() {
    companion object {
        fun createAndExecute(createMessage: MessageCreateFunction, connectionManager: ConnectionManager?, toSendMessages: BlockingQueue<ServerMessage>): MessageSendTask {
            val task = MessageSendTask()
            val taskArgs = MessageSendTaskArgs(createMessage, connectionManager, toSendMessages)
            task.execute(taskArgs)
            return task
        }
    }


    override fun doInBackground(vararg params: MessageSendTaskArgs?): Void? {
        val (createMessage, connectionManager, toSendMessages) = params[0]!!
        doInBackgroundBody(createMessage, connectionManager, toSendMessages)
        return null
    }

    private fun doInBackgroundBody(createMessage: MessageCreateFunction, connectionManager: ConnectionManager?, toSendMessages: BlockingQueue<ServerMessage>) {
        if (connectionManager == null || !connectionManager.isConnected()) {
            return
        }

        connectionManager.lock.read {
            if (!connectionManager.isConnected()) {
                return@read
            }

            val address = connectionManager.getConnectedAddress()
            toSendMessages.add(createMessage(address))
        }
    }
}
