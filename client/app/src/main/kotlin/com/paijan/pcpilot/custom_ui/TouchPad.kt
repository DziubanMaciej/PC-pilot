package com.paijan.pcpilot.custom_ui

import android.annotation.SuppressLint
import android.content.Context
import android.util.AttributeSet
import android.view.MotionEvent
import android.widget.ImageView


class TouchPad(context: Context, attrs: AttributeSet) : ImageView(context, attrs) {
    private var lastX = 0.0f
    private var lastY = 0.0f
    private var deltaX = 0.0f
    private var deltaY = 0.0f

    var onSendCursorMoveCallback: ((Float, Float) -> Unit)? = null

    @SuppressLint("ClickableViewAccessibility")
    override fun onTouchEvent(event: MotionEvent?): Boolean {

        when (event?.action) {
            MotionEvent.ACTION_DOWN -> {
                updatePosition(event.x, event.y)
                return true
            }
            MotionEvent.ACTION_MOVE -> {
                updatePosition(event.x, event.y)
                onSendCursorMoveCallback?.invoke(deltaX, deltaY)
                return true
            }
            else -> {
                return false
            }
        }
    }

    private fun updatePosition(x: Float, y: Float) {
        deltaX = x - lastX
        deltaY = y - lastY
        lastX = x
        lastY = y
    }
}