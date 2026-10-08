package com.airwave.cast.protocol

import java.nio.ByteBuffer
import java.nio.ByteOrder

object AwtpPacketizer {
    const val AWTP_MAGIC: Short = 0x4157
    const val AWTP_VERSION: Byte = 0x01
    const val MSG_TYPE_VIDEO: Byte = 0x10
    const val MSG_TYPE_AUDIO: Byte = 0x20
    const val FLAG_KEYFRAME: Byte = 0x01
    const val FLAG_SPS_PPS: Byte = 0x04
    const val HEADER_SIZE = 16

    fun wrapPacket(
        streamId: Byte,
        msgType: Byte,
        flags: Byte,
        sequence: Short,
        timestampUs: Int,
        payload: ByteArray,
        offset: Int,
        length: Int
    ): ByteArray {
        val totalSize = HEADER_SIZE + length
        val buffer = ByteBuffer.allocate(totalSize).order(ByteOrder.BIG_ENDIAN)

        buffer.putShort(AWTP_MAGIC)
        buffer.put(AWTP_VERSION)
        buffer.put(streamId)
        buffer.put(msgType)
        buffer.put(flags)
        buffer.putShort(sequence)
        buffer.putInt(timestampUs)
        buffer.putShort(length.toShort())
        buffer.putShort(0.toShort()) // reserved

        buffer.put(payload, offset, length)
        return buffer.array()
    }
}
