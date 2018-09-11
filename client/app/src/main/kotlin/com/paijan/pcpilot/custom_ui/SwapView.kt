package com.paijan.pcpilot.custom_ui;

import android.content.Context
import android.util.AttributeSet
import android.view.InflateException
import android.view.View
import android.widget.LinearLayout

open class SwapView : LinearLayout {
    constructor(context: Context?) : super(context)
    constructor(context: Context?, attrs: AttributeSet?) : super(context, attrs)
    constructor(context: Context?, attrs: AttributeSet?, defStyleAttr: Int) : super(context, attrs, defStyleAttr)

    fun showView(index: Int) {
        for (i in 0 until childCount) {
            getChildAt(i).visibility = View.GONE
        }
        getChildAt(index).visibility = View.VISIBLE
    }
}

class DualSwapView : SwapView {
    constructor(context: Context?) : super(context)
    constructor(context: Context?, attrs: AttributeSet?) : super(context, attrs)
    constructor(context: Context?, attrs: AttributeSet?, defStyleAttr: Int) : super(context, attrs, defStyleAttr)

    override fun onFinishInflate() {
        super.onFinishInflate()
        if (childCount != 2) throw InflateException("DualSwapView needs 2 subviews")
    }

    fun showEmptyView() {
        showView(0)
    }

    fun showFullView() {
        showView(1)
    }

    fun showView(full : Boolean) {
        if (full) showFullView()
        else showEmptyView()
    }
}
