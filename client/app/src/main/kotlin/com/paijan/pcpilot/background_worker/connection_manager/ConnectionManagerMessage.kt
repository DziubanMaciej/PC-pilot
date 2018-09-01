package com.paijan.pcpilot.background_worker.connection_manager

import java.net.InetSocketAddress

open class ConnectionManagerMessage(val address: InetSocketAddress)

class ConnectionManagerSendConnectionRequestMessage(address: InetSocketAddress) : ConnectionManagerMessage(address)
class ConnectionManagerKeepAliveMessage(address: InetSocketAddress) : ConnectionManagerMessage(address)
