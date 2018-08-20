package com.paijan.pcpilot.inetcommunicator

abstract class Message<T>(val data: T)
typealias AnyMessage = Message<out Any>

class BindMessage(data: BindMessageData) : Message<BindMessage.BindMessageData>(data) {
    data class BindMessageData(val ip: String, val port: Int)
}

class ConnectMessage(data: ConnectMessageData) : Message<ConnectMessage.ConnectMessageData>(data) {
    data class ConnectMessageData(val ip: String, val port: Int)
}

class SendMessage(data: String) : Message<String>(data)

class EndMessage : Message<Unit>(Unit)
