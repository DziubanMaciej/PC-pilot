package com.paijan.pcpilot.utils

import android.app.Activity
import android.content.Context
import android.view.KeyEvent
import android.view.WindowManager
import android.view.inputmethod.InputMethodManager
import com.paijan.pcpilot.background_worker.connection_manager.ConnectionManager
import java.util.concurrent.BlockingQueue

class Keyboard(private val activity: Activity) {
    fun toggleShow() {
        activity.window.setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_PAN)
        val imm = activity.getSystemService(Context.INPUT_METHOD_SERVICE) as InputMethodManager
        imm.toggleSoftInput(InputMethodManager.SHOW_IMPLICIT, InputMethodManager.HIDE_IMPLICIT_ONLY)
    }

    fun onKeyDown(event: KeyEvent?, connectionManager: ConnectionManager?, toSendMessages: BlockingQueue<ServerMessage>): Boolean {
        val messageCreateFunction = getMessageCreateFunction(event!!)
        if (messageCreateFunction != null) {
            MessageSendTask.createAndExecute(messageCreateFunction, connectionManager, toSendMessages)
            return true
        }
        return false
    }

    private fun getMessageCreateFunction(event: KeyEvent): MessageCreateFunction? {
        return when {
            event.keyCode == KeyEvent.KEYCODE_ENTER -> {
                { ServerMessage.createMessageKeyPressEnter(it) }
            }
            event.keyCode == KeyEvent.KEYCODE_DEL -> {
                { ServerMessage.createMessageKeyPressBackspace(it) }
            }
            event.unicodeChar > 0 -> {
                { ServerMessage.createMessageKeyPress(it, event.unicodeChar) }
            }
            else -> {
                null
            }
        }
    }
}
