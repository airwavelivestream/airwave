package com.airwave.cast.ratecontrol

import android.media.MediaCodec
import android.os.Bundle
import android.util.Log

class AdaptiveRateController(
    private val minBitrate: Int = 4_000_000,   // 4 Mbps floor
    private val maxBitrate: Int = 24_000_000,  // 24 Mbps ceiling
    private var currentBitrate: Int = 12_000_000
) {
    private var lastAdjustmentTime = System.currentTimeMillis()

    fun onNetworkFeedback(
        rttMs: Double,
        packetLossPercent: Double,
        codec: MediaCodec?
    ) {
        val now = System.currentTimeMillis()
        if (now - lastAdjustmentTime < 500) {
            return // Dampen adjustments to 500ms intervals
        }

        var newBitrate = currentBitrate

        // AIMD (Additive Increase / Multiplicative Decrease) Rate Control
        if (packetLossPercent > 2.0 || rttMs > 80.0) {
            // Congestion or packet loss detected: Multiplicative decrease (-20%)
            newBitrate = (currentBitrate * 0.80).toInt().coerceAtLeast(minBitrate)
            Log.w("RateController", "Congestion detected (Loss: $packetLossPercent%, RTT: ${rttMs}ms) -> Downscaling bitrate to ${newBitrate / 1_000_000} Mbps")
        } else if (packetLossPercent < 0.5 && rttMs < 35.0) {
            // Stable link: Additive increase (+1 Mbps)
            newBitrate = (currentBitrate + 1_000_000).coerceAtMost(maxBitrate)
            Log.d("RateController", "Channel clear -> Scaling bitrate to ${newBitrate / 1_000_000} Mbps")
        }

        if (newBitrate != currentBitrate) {
            currentBitrate = newBitrate
            lastAdjustmentTime = now
            applyBitrate(codec, currentBitrate)
        }
    }

    private fun applyBitrate(codec: MediaCodec?, bitrate: Int) {
        if (codec == null) return
        try {
            val params = Bundle().apply {
                putInt(MediaCodec.PARAMETER_KEY_VIDEO_BITRATE, bitrate)
            }
            codec.setParameters(params)
        } catch (e: Exception) {
            Log.e("RateController", "Failed to update dynamic bitrate: ${e.message}")
        }
    }

    fun getCurrentBitrate(): Int = currentBitrate
}
