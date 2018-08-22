package com.paijan.pcpilot.background_worker

import android.util.Log

abstract class RunnableAdapter(private val messageTag: String) : Runnable {
    override fun run() {
        Log.i(messageTag, "start")
        try {
            while (true) {
                runBody()
            }
        } catch (e: InterruptedException) {
            Log.e(messageTag, "Thread interrupted")
        }
        Log.i(messageTag, "end")
    }

    @Throws(InterruptedException::class)
    protected abstract fun runBody()
}