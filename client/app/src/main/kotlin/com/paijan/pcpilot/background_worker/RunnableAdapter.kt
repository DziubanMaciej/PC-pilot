package com.paijan.pcpilot.background_worker

import android.util.Log

abstract class RunnableAdapter(protected val messageTag: String) : Runnable {
    override fun run() {
        Log.i(messageTag, "Start, id=" + Thread.currentThread().id)
        try {
            while (!Thread.interrupted()) {
                runBody()
            }
        } catch (e: InterruptedException) {
        }
        Log.i(messageTag, "End, id=" + Thread.currentThread().id)
    }

    @Throws(InterruptedException::class)
    protected abstract fun runBody()
}