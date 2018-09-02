package com.paijan.pcpilot.activity

import android.app.Application
import com.paijan.pcpilot.utils.DatagramSocketTuple

class ApplicationImpl : Application() {
    var sockets: DatagramSocketTuple? = null
}
