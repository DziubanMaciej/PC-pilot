package com.paijan.pcpilot.custom_ui

import android.annotation.SuppressLint
import android.content.Context
import android.util.AttributeSet
import android.view.MotionEvent
import android.widget.ImageButton

class TouchPadButton(context: Context, attrs: AttributeSet) : ImageButton(context, attrs) {
    var onTouchDown: (() -> Unit)? = null
    var onTouchUp: (() -> Unit)? = null

    @SuppressLint("ClickableViewAccessibility")
    override fun onTouchEvent(event: MotionEvent?): Boolean {
        return when (event?.action) {
            MotionEvent.ACTION_DOWN -> {
                onTouchDown?.invoke()
                true
            }
            MotionEvent.ACTION_UP -> {
                onTouchUp?.invoke()
                true
            }
            else -> {
                super.onTouchEvent(event)
            }
        }
    }
}