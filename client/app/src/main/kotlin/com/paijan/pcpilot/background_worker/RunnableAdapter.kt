package com.paijan.pcpilot.background_worker

import android.util.Log

abstract class RunnableAdapter(protected val messageTag: String) : Runnable {
    override fun run() {
        Log.i(messageTag, "start id=" + Thread.currentThread().id)
        try {
            while (true) {
                runBody()
            }
        } catch (e: InterruptedException) {
            Log.i(messageTag, "Thread interrupted")
        }
        Log.i(messageTag, "end")
    }

    @Throws(InterruptedException::class)
    protected abstract fun runBody()
}