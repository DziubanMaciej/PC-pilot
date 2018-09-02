package com.paijan.pcpilot.activity

import android.app.Application
import com.paijan.pcpilot.utils.SocketEstablisher

class ApplicationImpl : Application() {
    var sockets: SocketEstablisher.DatagramSocketTuple? = null
}
