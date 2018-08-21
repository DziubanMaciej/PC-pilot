// This file is autogenerated, do not try to change it manually
package com.paijan.pcpilot.communication

import java.net.InetAddress
import java.nio.ByteBuffer
import java.util.*

abstract class Message protected constructor(private val bytes: ByteArray) {
    protected fun getFloat(index: Int): Float {
        return ByteBuffer.wrap(bytes).getFloat(index)
    }

    protected fun getByte(index: Int): Byte {
        return bytes[index]
    }

    protected fun getBytes(from: Int, to: Int): ByteArray {
        return Arrays.copyOfRange(bytes, from, to)
    }

    protected fun getInt(index: Int): Int {
        return ByteBuffer.wrap(bytes).getInt(index)
    }

    protected fun getInetAddress(index: Int): InetAddress {
        val address = Arrays.copyOfRange(bytes, index, index + 4)
        return InetAddress.getByAddress(address)
    }
}



class ServerMessage private constructor(bytes : ByteArray) : Message(bytes) {
    enum class Type {
        MoveCursor,
        LeftPress,
        LeftRelease;

        fun byteValue() = ordinal.toByte()
    }

    companion object {
        private const val SIZE = 9

        fun createMessageMoveCursor(x: Float, y: Float) : ServerMessage {
            val bytes = ByteBuffer.allocate(SIZE)
                .put(Type.MoveCursor.byteValue())
                .putFloat(x)
                .putFloat(y)
                .array()
            return ServerMessage(bytes)
        }
        fun createMessageLeftPress() : ServerMessage {
            val bytes = ByteBuffer.allocate(SIZE)
                .put(Type.LeftPress.byteValue())
                .array()
            return ServerMessage(bytes)
        }
        fun createMessageLeftRelease() : ServerMessage {
            val bytes = ByteBuffer.allocate(SIZE)
                .put(Type.LeftRelease.byteValue())
                .array()
            return ServerMessage(bytes)
        }
    }
}



class ClientMessage private constructor(bytes : ByteArray) : Message(bytes) {
    enum class Type {
        Connect;

        fun byteValue() = ordinal.toByte()
    }

    companion object {
        private const val SIZE = 1

        fun createMessageConnect() : ClientMessage {
            val bytes = ByteBuffer.allocate(SIZE)
                .put(Type.Connect.byteValue())
                .array()
            return ClientMessage(bytes)
        }
    }
}
