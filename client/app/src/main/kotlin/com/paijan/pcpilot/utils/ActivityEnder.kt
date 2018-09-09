package com.paijan.pcpilot.utils

import android.app.Activity

class ActivityEnder(private val activity: Activity) {
    private var activityEnded = false

    val onThreadEndCallback = { activity.runOnUiThread { onThreadEnd() } }

    @Synchronized
    fun onThreadEnd() {
        if (activityEnded) return
        endActivity()
        activityEnded = true
    }

    @Synchronized
    fun endActivity() {
        activityEnded = true
        activity.finish()
    }

    @Synchronized
    fun endApplication() {
        activityEnded = true
        activity.finishAffinity()
    }
}