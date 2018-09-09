package com.paijan.pcpilot.background_worker

import android.util.Log

typealias ThreadEndCallback = () -> Unit

abstract class RunnableAdapter(
        protected val messageTag: String,
        private val onThreadEnd: ThreadEndCallback
) : Runnable {
    override fun run() {
        try {
            Log.i(messageTag, "Start, id=" + Thread.currentThread().id)
            try {
                while (!Thread.interrupted()) {
                    runBody()
                }
            } catch (e: InterruptedException) {
            }
            Log.i(messageTag, "End, id=" + Thread.currentThread().id)
            onThreadEnd()
        } catch (e: Throwable) {
            Log.e(messageTag, "Uncaught exception in thread with id=" + Thread.currentThread().id, e)
            throw e
        }
    }

    @Throws(InterruptedException::class)
    protected abstract fun runBody()
}