package com.paijan.pcpilot.activity;

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.util.Log
import com.paijan.pcpilot.R
import com.paijan.pcpilot.utils.DatagramSocketTuple
import com.paijan.pcpilot.utils.SocketEstablisher

class SocketEstablishActivity : Activity() {
    private var socketEstablisherThread: Thread? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_establish)
    }

    override fun onResume() {
        super.onResume()
        Log.i("SocketEstablishActivity", "onResume() - begin establishing")
        socketEstablisherThread = Thread(SocketEstablisher { onSocketsEstablished(it) }).apply { start()}
    }

    private fun onSocketsEstablished(sockets: DatagramSocketTuple) {
        Log.i("SocketEstablishActivity", "onSocketsEstablished()")

        socketEstablisherThread?.interrupt()
        socketEstablisherThread = null

        (application as ApplicationImpl).sockets = sockets
        val intent = Intent(this, MainActivity::class.java)
        startActivity(intent)
    }
}
