package com.airwave.cast.audio

import android.annotation.SuppressLint
import android.media.AudioAttributes
import android.media.AudioFormat
import android.media.AudioPlaybackCaptureConfiguration
import android.media.AudioRecord
import android.media.projection.MediaProjection
import android.os.Build
import androidx.annotation.RequiresApi
import com.airwave.cast.protocol.AwtpPacketizer
import java.net.DatagramPacket
import java.net.DatagramSocket
import java.net.InetAddress
import java.nio.ByteBuffer

@RequiresApi(Build.VERSION_CODES.Q)
class AudioCaptureEngine(
    private val targetHost: String,
    private val targetPort: Int = 49152
) {
    private var audioRecord: AudioRecord? = null
    private var isRecording = false
    private var workerThread: Thread? = null
    private var udpSocket: DatagramSocket? = null
    private var targetInetAddress: InetAddress? = null
    private var sequenceNumber: Short = 0

    companion object {
        const val SAMPLE_RATE = 48000
        const val CHANNEL_CONFIG = AudioFormat.CHANNEL_IN_STEREO
        const val AUDIO_FORMAT = AudioFormat.ENCODING_PCM_16BIT
    }

    @SuppressLint("MissingPermission")
    fun start(mediaProjection: MediaProjection) {
        if (isRecording) return
        isRecording = true

        udpSocket = DatagramSocket()
        targetInetAddress = InetAddress.getByName(targetHost)

        val config = AudioPlaybackCaptureConfiguration.Builder(mediaProjection)
            .addMatchingUsage(AudioAttributes.USAGE_GAME)
            .addMatchingUsage(AudioAttributes.USAGE_MEDIA)
            .build()

        val bufferSize = AudioRecord.getMinBufferSize(SAMPLE_RATE, CHANNEL_CONFIG, AUDIO_FORMAT) * 2

        audioRecord = AudioRecord.Builder()
            .setAudioFormat(
                AudioFormat.Builder()
                    .setEncoding(AUDIO_FORMAT)
                    .setSampleRate(SAMPLE_RATE)
                    .setChannelMask(CHANNEL_CONFIG)
                    .build()
            )
            .setAudioPlaybackCaptureConfig(config)
            .setBufferSizeInBytes(bufferSize)
            .build()

        audioRecord?.startRecording()

        workerThread = Thread {
            val audioBuffer = ByteArray(1920) // 10ms of 48kHz stereo 16-bit PCM

            while (isRecording) {
                val readBytes = audioRecord?.read(audioBuffer, 0, audioBuffer.size) ?: 0
                if (readBytes > 0) {
                    val timestampUs = (System.nanoTime() / 1000 and 0xFFFFFFFFL).toInt()
                    val packet = AwtpPacketizer.wrapPacket(
                        streamId = 0,
                        msgType = AwtpPacketizer.MSG_TYPE_AUDIO,
                        flags = 0,
                        sequence = sequenceNumber++,
                        timestampUs = timestampUs,
                        payload = audioBuffer,
                        offset = 0,
                        length = readBytes
                    )
                    try {
                        val dPacket = DatagramPacket(packet, packet.size, targetInetAddress, targetPort)
                        udpSocket?.send(dPacket)
                    } catch (e: Exception) {
                        e.printStackTrace()
                    }
                }
            }
        }.apply { start() }
    }

    fun stop() {
        isRecording = false
        try {
            audioRecord?.stop()
            audioRecord?.release()
            audioRecord = null
            udpSocket?.close()
            workerThread?.interrupt()
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }
}
