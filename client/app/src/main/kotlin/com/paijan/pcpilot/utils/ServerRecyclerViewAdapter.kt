package com.paijan.pcpilot.utils

import android.app.Activity
import android.support.v7.widget.RecyclerView
import android.view.ViewGroup
import android.widget.Button
import com.paijan.pcpilot.custom_ui.DualSwapView
import java.net.InetSocketAddress

class ServerRecyclerViewAdapter(private val dualSwapView: DualSwapView)
    : RecyclerView.Adapter<ServerRecyclerViewAdapter.ServerRecyclerViewHolder>() {
    class ServerRecyclerViewHolder(val view: Button) : RecyclerView.ViewHolder(view)
    data class DataEntry(val timestamp: Long, val address: InetSocketAddress)

    private val data = mutableListOf<DataEntry>()

    fun addEntry(address: InetSocketAddress) {
        val dataEntry = DataEntry(System.currentTimeMillis(), address)
        data.add(dataEntry)
        onDataSetChanged()
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

    private fun onDataSetChanged() {
        (dualSwapView.context as Activity).runOnUiThread {
            val isFull = itemCount > 0
            dualSwapView.showView(isFull)
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
