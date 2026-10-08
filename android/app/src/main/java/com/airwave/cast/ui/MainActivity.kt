package com.airwave.cast.ui

import android.app.Activity
import android.content.Context
import android.content.Intent
import android.media.projection.MediaProjectionManager
import android.os.Bundle
import android.os.PowerManager
import android.provider.Settings
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.airwave.cast.service.AirwaveCastService

class MainActivity : ComponentActivity() {
    private var isCasting by mutableStateOf(false)
    private var targetHost by mutableStateOf("192.168.42.2") // Typical Windows USB RNDIS host IP

    private val projectionLauncher = registerForActivityResult(
        ActivityResultContracts.StartActivityForResult()
    ) { result ->
        if (result.resultCode == Activity.RESULT_OK && result.data != null) {
            val intent = Intent(this, AirwaveCastService::class.java).apply {
                putExtra(AirwaveCastService.EXTRA_RESULT_CODE, result.resultCode)
                putExtra(AirwaveCastService.EXTRA_RESULT_DATA, result.data)
                putExtra(AirwaveCastService.EXTRA_TARGET_HOST, targetHost)
            }
            startForegroundService(intent)
            isCasting = true
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestBatteryOptimizationExemption()

        setContent {
            AirwaveCastAppTheme {
                AirwaveMainScreen(
                    isCasting = isCasting,
                    targetHost = targetHost,
                    onHostChange = { targetHost = it },
                    onToggleCast = {
                        if (isCasting) {
                            stopService(Intent(this, AirwaveCastService::class.java))
                            isCasting = false
                        } else {
                            val manager = getSystemService(Context.MEDIA_PROJECTION_SERVICE) as MediaProjectionManager
                            projectionLauncher.launch(manager.createScreenCaptureIntent())
                        }
                    }
                )
            }
        }
    }

    private fun requestBatteryOptimizationExemption() {
        val powerManager = getSystemService(Context.POWER_SERVICE) as PowerManager
        if (!powerManager.isIgnoringBatteryOptimizations(packageName)) {
            val intent = Intent(Settings.ACTION_IGNORE_BATTERY_OPTIMIZATION_SETTINGS)
            startActivity(intent)
        }
    }
}

@Composable
fun AirwaveCastAppTheme(content: @Composable () -> Unit) {
    MaterialTheme(
        colorScheme = darkColorScheme(
            background = Color(0xFF07080C),
            surface = Color(0xFF11131A),
            primary = Color(0xFF19E3FF),
            secondary = Color(0xFF7C5CFF)
        ),
        content = content
    )
}

@Composable
fun AirwaveMainScreen(
    isCasting: Boolean,
    targetHost: String,
    onHostChange: (String) -> Unit,
    onToggleCast: () -> Unit
) {
    Surface(
        modifier = Modifier.fillMaxSize(),
        color = Color(0xFF07080C)
    ) {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(24.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.SpaceBetween
        ) {
            // Header
            Column(horizontalAlignment = Alignment.CenterHorizontally) {
                Spacer(modifier = Modifier.height(24.dp))
                Text(
                    text = "AIRWAVE",
                    fontSize = 28.sp,
                    fontWeight = FontWeight.Bold,
                    color = Color(0xFF19E3FF),
                    letterSpacing = 2.sp
                )
                Text(
                    text = "Zero Lag Mobile Game Mirror",
                    fontSize = 13.sp,
                    color = Color(0xFF8E929E)
                )
            }

            // Central Pulsing Glow Button
            Box(
                contentAlignment = Alignment.Center,
                modifier = Modifier
                    .size(240.dp)
                    .border(
                        width = 2.dp,
                        brush = Brush.radialGradient(
                            listOf(
                                if (isCasting) Color(0xFFB6FF3B) else Color(0xFF19E3FF),
                                Color.Transparent
                            )
                        ),
                        shape = CircleShape
                    )
            ) {
                Button(
                    onClick = onToggleCast,
                    modifier = Modifier.size(180.dp),
                    shape = CircleShape,
                    colors = ButtonDefaults.buttonColors(
                        containerColor = if (isCasting) Color(0xFFFF6B35) else Color(0xFF7C5CFF)
                    )
                ) {
                    Text(
                        text = if (isCasting) "STOP CAST" else "START CAST",
                        fontSize = 18.sp,
                        fontWeight = FontWeight.Bold,
                        color = Color.White
                    )
                }
            }

            // Tether Connection Setup Card
            Card(
                modifier = Modifier.fillMaxWidth(),
                shape = RoundedCornerShape(12.dp),
                colors = CardDefaults.cardColors(containerColor = Color(0xFF11131A))
            ) {
                Column(modifier = Modifier.padding(16.dp)) {
                    Text(
                        text = "WIRED USB TETHERING",
                        fontSize = 12.sp,
                        fontWeight = FontWeight.Bold,
                        color = Color(0xFF19E3FF)
                    )
                    Spacer(modifier = Modifier.height(8.dp))
                    OutlinedTextField(
                        value = targetHost,
                        onValueChange = onHostChange,
                        label = { Text("PC Tether IP") },
                        singleLine = true,
                        modifier = Modifier.fillMaxWidth()
                    )
                    Spacer(modifier = Modifier.height(8.dp))
                    Text(
                        text = "Tip: Enable 'USB Tethering' in Android Settings for lowest latency (<50ms).",
                        fontSize = 11.sp,
                        color = Color(0xFF8E929E)
                    )
                }
            }
        }
    }
}
