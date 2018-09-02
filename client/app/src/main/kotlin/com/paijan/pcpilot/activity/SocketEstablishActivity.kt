package com.paijan.pcpilot.activity;

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.view.View
import com.paijan.pcpilot.R
import com.paijan.pcpilot.utils.SocketEstablisher

class SocketEstablishActivity : Activity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_establish)
    }

    override fun onResume() {
        super.onResume()

        val address = SocketEstablisher.getLocalAddress()
        if (address == null) {
            // TODO textfield
            Log.e("MainActivity", "No local address")
            return
        }

        val sockets = SocketEstablisher.establishSockets(address)
        if (sockets == null) {
            // TODO textfield
            Log.e("MainActivity", "Socket establishing failed")
            return
        }
        (application as ApplicationImpl).sockets = sockets
    }

    fun onClickB(v : View?) {
        val intent = Intent(this, MainActivity::class.java)
        startActivity(intent)
    }
}
