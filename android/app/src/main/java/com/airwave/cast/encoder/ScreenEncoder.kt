package com.airwave.cast.encoder

import android.hardware.display.DisplayManager
import android.hardware.display.VirtualDisplay
import android.media.MediaCodec
import android.media.MediaCodecInfo
import android.media.MediaFormat
import android.media.projection.MediaProjection
import android.os.Handler
import android.os.HandlerThread
import android.view.Surface
import com.airwave.cast.protocol.AwtpPacketizer
import java.io.OutputStream
import java.net.DatagramPacket
import java.net.DatagramSocket
import java.net.InetAddress
import java.nio.ByteBuffer

class ScreenEncoder(
    private val width: Int = 1920,
    private val height: Int = 1080,
    private val dpi: Int = 400,
    private val frameRate: Int = 60,
    private val bitRate: Int = 12_000_000, // 12 Mbps default for PUBG
    private val targetHost: String,
    private val targetPort: Int = 49152
) {
    private var mediaCodec: MediaCodec? = null
    private var inputSurface: Surface? = null
    private var virtualDisplay: VirtualDisplay? = null
    private var udpSocket: DatagramSocket? = null
    private var targetInetAddress: InetAddress? = null

    private var handlerThread: HandlerThread? = null
    private var handler: Handler? = null

    private var sequenceNumber: Short = 0
    private var isRunning = false

    fun start(mediaProjection: MediaProjection) {
        if (isRunning) return
        isRunning = true

        udpSocket = DatagramSocket()
        udpSocket?.sendBufferSize = 4 * 1024 * 1024
        targetInetAddress = InetAddress.getByName(targetHost)

        handlerThread = HandlerThread("AirwaveEncoderThread").apply { start() }
        handler = Handler(handlerThread!!.looper)

        val format = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC, width, height).apply {
            setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface)
            setInteger(MediaFormat.KEY_BIT_RATE, bitRate)
            setInteger(MediaFormat.KEY_FRAME_RATE, frameRate)
            setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1) // 1 second keyframe interval

            // Low Latency Keys (Snapdragon / MediaTek Zero-Latency mode)
            setInteger(MediaFormat.KEY_LATENCY, 0)
            setInteger(MediaFormat.KEY_PRIORITY, 0) // Real-time
            setInteger("bitrate-mode", MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_CBR)
            setInteger("vendor.qti-ext-enc-low-latency.enable", 1) // Qualcomm Snapdragon low latency extension
        }

        mediaCodec = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_VIDEO_AVC)
        mediaCodec?.setCallback(object : MediaCodec.Callback() {
            override fun onInputBufferAvailable(codec: MediaCodec, index: Int) {
                // Surface input handles buffers automatically
            }

            override fun onOutputBufferAvailable(codec: MediaCodec, index: Int, info: MediaCodec.BufferInfo) {
                if (!isRunning) return
                val outputBuffer = codec.getOutputBuffer(index) ?: return

                if ((info.flags and MediaCodec.BUFFER_FLAG_CODEC_CONFIG) != 0) {
                    // Send SPS/PPS header
                    sendEncodedChunk(outputBuffer, info, AwtpPacketizer.FLAG_SPS_PPS)
                    codec.releaseOutputBuffer(index, false)
                    return
                }

                val isKey = (info.flags and MediaCodec.BUFFER_FLAG_KEY_FRAME) != 0
                val flag: Byte = if (isKey) AwtpPacketizer.FLAG_KEYFRAME else 0x00
                sendEncodedChunk(outputBuffer, info, flag)
                codec.releaseOutputBuffer(index, false)
            }

            override fun onError(codec: MediaCodec, e: MediaCodec.CodecException) {
                e.printStackTrace()
            }

            override fun onOutputFormatChanged(codec: MediaCodec, format: MediaFormat) {
            }
        }, handler)

        mediaCodec?.configure(format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE)
        inputSurface = mediaCodec?.createInputSurface()
        mediaCodec?.start()

        virtualDisplay = mediaProjection.createVirtualDisplay(
            "AirwaveVirtualDisplay",
            width,
            height,
            dpi,
            DisplayManager.VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR,
            inputSurface,
            null,
            handler
        )
    }

    private fun sendEncodedChunk(buffer: ByteBuffer, info: MediaCodec.BufferInfo, flag: Byte) {
        val payload = ByteArray(info.size)
        buffer.position(info.offset)
        buffer.get(payload, 0, info.size)

        val timestampUs = (info.presentationTimeUs and 0xFFFFFFFFL).toInt()
        val packetData = AwtpPacketizer.wrapPacket(
            streamId = 0,
            msgType = AwtpPacketizer.MSG_TYPE_VIDEO,
            flags = flag,
            sequence = sequenceNumber++,
            timestampUs = timestampUs,
            payload = payload,
            offset = 0,
            length = info.size
        )

        try {
            val dPacket = DatagramPacket(packetData, packetData.size, targetInetAddress, targetPort)
            udpSocket?.send(dPacket)
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    fun stop() {
        isRunning = false
        try {
            virtualDisplay?.release()
            mediaCodec?.stop()
            mediaCodec?.release()
            inputSurface?.release()
            udpSocket?.close()
            handlerThread?.quitSafely()
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }
}
