package com.paijan.pcpilot.utils

import android.app.Activity
import android.support.v7.widget.RecyclerView
import android.util.Log
import android.view.LayoutInflater
import android.view.ViewGroup
import android.widget.Button
import com.paijan.pcpilot.R
import com.paijan.pcpilot.custom_ui.SwapView
import kotlinx.android.synthetic.main.activity_main.view.*
import java.net.InetSocketAddress

class ServerRecyclerViewAdapter(
        private val swapView: SwapView,
        private val onCheckConnectionState: () -> Boolean,
        private val onConnectCallback: (InetSocketAddress) -> Unit
) : RecyclerView.Adapter<ServerRecyclerViewAdapter.ServerRecyclerViewHolder>() {
    class ServerRecyclerViewHolder(val view: Button) : RecyclerView.ViewHolder(view)
    data class DataEntry(var timestamp: Long, val address: InetSocketAddress)

    private val data = mutableListOf<DataEntry>()

    fun addEntry(address: InetSocketAddress) {
        val timestamp = System.currentTimeMillis()
        val existingDataEntry = data.find { it.address == address }
        if (existingDataEntry != null) {
            existingDataEntry.timestamp = timestamp
        } else {
            val newDataEntry = DataEntry(timestamp, address)
            data.add(newDataEntry)
            onDataSetChanged()
        }
    }

    fun clearOldEntries(maxEntryAge: Long): Int {
        var entriesRemoved = 0
        val minimumTimestamp = System.currentTimeMillis() - maxEntryAge
        val iterator = data.iterator()
        while (iterator.hasNext()) {
            if (iterator.next().timestamp <= minimumTimestamp) {
                iterator.remove()
                entriesRemoved++
            }
        }
        onDataSetChanged()
        return entriesRemoved
    }

    fun onDataSetChanged() {
        (swapView.context as Activity).runOnUiThread {
            notifyDataSetChanged()
            if (onCheckConnectionState()) {
                swapView.showView(swapView.disconnectServerList)
                return@runOnUiThread
            }
            if (itemCount > 0) {
                swapView.showView(swapView.serverList)
                return@runOnUiThread
            }
            swapView.showView(swapView.emptyServerList)
        }
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ServerRecyclerViewHolder {
        val view = LayoutInflater.from(parent.context).inflate(R.layout.server_list_element, parent, false) as Button
        return ServerRecyclerViewHolder(view)
    }

    override fun getItemCount(): Int {
        return data.size
    }

    override fun onBindViewHolder(holder: ServerRecyclerViewHolder, position: Int) {
        val address = data[position].address
        holder.view.apply {
            text = address.toString()
            setOnClickListener { onConnectCallback(data[position].address) }
        }
    }
}
