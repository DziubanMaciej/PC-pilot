package com.paijan.pcpilot.utils

import android.app.Activity
import android.support.v7.widget.RecyclerView
import android.util.Log
import android.view.ViewGroup
import android.widget.Button
import com.paijan.pcpilot.custom_ui.SwapView
import kotlinx.android.synthetic.main.activity_main.view.*
import java.net.InetSocketAddress

class ServerRecyclerViewAdapter(private val swapView: SwapView, private val onCheckConnectionState: () -> Boolean)
    : RecyclerView.Adapter<ServerRecyclerViewAdapter.ServerRecyclerViewHolder>() {
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
        Log.i("XD", "onDataSetChanged")
        (swapView.context as Activity).runOnUiThread {
            notifyDataSetChanged()
            if (onCheckConnectionState()) {
                swapView.showView(swapView.buttonDisconnect)
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
        return ServerRecyclerViewHolder(Button(parent.context))
    }

    override fun getItemCount(): Int {
        return data.size
    }

    override fun onBindViewHolder(holder: ServerRecyclerViewHolder, position: Int) {
        holder.view.text = data[position].address.toString()
    }
}
