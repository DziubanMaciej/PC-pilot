package com.paijan.pcpilot

import java.net.Socket

import android.app.Activity
import android.os.Bundle
import android.view.View
import com.paijan.pcpilot.inetcommunicator.InetCommunicator
import java.net.InetAddress

class MainActivity : Activity() {
    private val socket: Socket? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
    }

    fun onClick1(v: View?) {
        val filterFunction = object : FilterFunction {
            override fun filter(address: InetAddress): Boolean {
                return address.hostAddress.startsWith("192")
            }
        }
        val addresses = SocketEstablisher.getLocalAddresses(filterFunction)
        val myAddress = addresses.first().hostAddress
        val theirAddress = "192.168.0.60"

        val communicator = InetCommunicator()
        communicator.use {
            it.bind(myAddress, 0)
            it.connectWith(theirAddress, 8888)
            it.send("XD")
        }
        communicator.join()
    }

    fun onClick2(v: View?) {

    }

    fun onClick3(v: View?) {

    }
}
