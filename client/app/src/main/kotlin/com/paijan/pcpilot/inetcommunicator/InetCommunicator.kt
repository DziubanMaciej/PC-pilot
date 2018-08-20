package com.paijan.pcpilot.inetcommunicator

import android.util.Log
import java.io.OutputStreamWriter
import java.io.Writer
import java.net.InetSocketAddress
import java.net.Socket
import java.util.concurrent.BlockingQueue
import java.util.concurrent.LinkedBlockingQueue
import kotlin.concurrent.thread

class InetCommunicator : AutoCloseable {
    private val messageQueue: BlockingQueue<AnyMessage> = LinkedBlockingQueue()
    private val socket: Socket = Socket()
    private var thread: Thread? = null
    private var out: Writer? = null

    fun join() {
        thread?.join()
    }

    init {
        this.thread = thread { main() }
    }

    fun bind(address: String, port: Int) {
        put(BindMessage(BindMessage.BindMessageData(address, port)))
    }

    fun connectWith(address: String, port: Int) {
        put(ConnectMessage(ConnectMessage.ConnectMessageData(address, port)))
    }

    fun <T> send(what: T) {
        put(SendMessage(what.toString()))
    }

    override fun close() {
        put(EndMessage())
    }

    private fun put(message: AnyMessage) {
        messageQueue.put(message)
    }

    private fun log(text: String, message: AnyMessage? = null) {
        var logText: String = text
        if (message != null) {
            logText += ": " + message.data.toString()
        }
        Log.i("Worker", logText)
    }

    private fun main() {
        var running = true
        while (running) {
            println("BG_THREAD: waiting for messages...")
            val message = messageQueue.take()

            when (message) {
                is BindMessage -> {
                    log("bind", message)
                    val socketAddress = InetSocketAddress(message.data.ip, message.data.port)
                    socket.bind(socketAddress)
                }
                is ConnectMessage -> {
                    log("connect", message)
                    val socketAddress = InetSocketAddress(message.data.ip, message.data.port)
                    socket.connect(socketAddress)
                    out = OutputStreamWriter(socket.getOutputStream())
                    Thread.sleep(500)
                }
                is SendMessage -> {
                    log("send", message)
                    out!!.write(message.data)
                    Thread.sleep(500)
                }
                is EndMessage -> {
                    log("end")
                    out?.close()
                    socket.close()
                    log("end, socket connected = " + socket.isConnected)
                    running = false
                }
                else -> println("WORKER: unknown message")
            }
        }
    }

}