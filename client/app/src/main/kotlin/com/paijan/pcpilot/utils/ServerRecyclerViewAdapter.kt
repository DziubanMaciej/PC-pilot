package com.paijan.pcpilot.utils

import android.support.v7.widget.RecyclerView
import android.view.ViewGroup
import android.widget.Button
import com.paijan.pcpilot.custom_ui.DualSwapView
import java.net.InetSocketAddress

class ServerRecyclerViewAdapter(private val dualSwapView: DualSwapView) : RecyclerView.Adapter<ServerRecyclerViewAdapter.ServerRecyclerViewHolder>() {
    class ServerRecyclerViewHolder(val view: Button) : RecyclerView.ViewHolder(view)

    val data = mutableListOf<InetSocketAddress>()

    init {
        registerAdapterDataObserver(object : RecyclerView.AdapterDataObserver() {
            override fun onChanged() {
                val isFull = itemCount > 0
                dualSwapView.showView(isFull)
            }
        })
        notifyDataSetChanged()
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ServerRecyclerViewHolder {
        return ServerRecyclerViewHolder(Button(parent.context))
    }

    override fun getItemCount(): Int {
        return data.size
    }

    override fun onBindViewHolder(holder: ServerRecyclerViewHolder, position: Int) {
        holder.view.text = data[position].toString()
    }
}