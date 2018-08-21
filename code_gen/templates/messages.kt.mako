<%! import utils.template_utils as utils %>\
package com.paijan.pcpilot.inetcommunicator

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

%for message_class in message_classes:


class ${message_class.name} private constructor(bytes : ByteArray) : Message(bytes) {
    enum class Type {
    % for message in message_class.messages:
        ${message.name}${utils.trailing_sign(loop.last, ',', ';')}
    % endfor

        fun byteValue() = ordinal.toByte()
    }

    companion object {
        private const val SIZE = ${utils.get_largest_message_size(message_class)}

        % for message in message_class.messages:
        fun createMessage${message.name}(${utils.get_args_list_kt(message.fields)}) : ${message_class.name} {
            val bytes = ByteBuffer.allocate(SIZE)
                ${utils.get_put_call_kt('Byte', 'Type.{}.byteValue()'.format(message.name))}
                % for field in message.fields:
                % if not utils.is_type_field(field):
                ${utils.get_put_call_kt(field.type, field.name)}
                % endif
                % endfor
                .array()
            return ${message_class.name}(bytes)
        }
        % endfor
    }
}

%endfor