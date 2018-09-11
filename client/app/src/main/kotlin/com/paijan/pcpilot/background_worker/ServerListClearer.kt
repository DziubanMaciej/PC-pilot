package com.paijan.pcpilot.background_worker

import android.util.Log
import com.paijan.pcpilot.utils.Constants
import com.paijan.pcpilot.utils.ServerRecyclerViewAdapter

class ServerListClearer(
        private val serverRecyclerViewAdapter: ServerRecyclerViewAdapter,
        onThreadEnd: ThreadEndCallback
) : RunnableAdapter("ServerListClearer", onThreadEnd) {

    @Throws(InterruptedException::class)
    override fun runBody() {
        Thread.sleep(Constants.SERVER_LIST_MAX_AGE_MS)
        val entriesRemoved = serverRecyclerViewAdapter.clearOldEntries(Constants.SERVER_LIST_MAX_AGE_MS)
        Log.i(messageTag, "Removed ${entriesRemoved} entries")
    }
}
