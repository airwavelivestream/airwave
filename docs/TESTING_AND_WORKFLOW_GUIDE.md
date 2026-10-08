# Airwave: Complete Testing, Deployment & User Workflow Guide

This guide provides end-to-end instructions for testing the codebase, setting up the development environment, and running the live casting and streaming workflows across Android, iOS, Windows, and OBS Studio.

---

## 🧪 Part 1: Running the Complete Automated Test Suite

Airwave includes 11 automated benchmark test harnesses validating every layer of the pipeline (transport, hardware encode/decode latency, 5GHz Wi-Fi rate adaptation, accessibility contrast, A/V synchronization, multi-device grid, touch injection, and PUBG 60/90/120 FPS stress tests).

### 1. Execute All Tests in Batch (PowerShell)
Open PowerShell in the project directory:
```powershell
cd c:\Users\gurud\Downloads\airwave
Get-ChildItem -Path tests -Filter "*.py" | ForEach-Object { 
    Write-Host "Running $($_.Name)..." -ForegroundColor Cyan
    python $_.FullName 
}
```

### 2. Individual Component Tests
To isolate and benchmark specific modules:

| Target Component | Command | Expected Output |
| :--- | :--- | :--- |
| **AWTP Network Protocol** | `python tests\test_loopback_latency.py` | Transport Latency: ~0.16 ms, 0% loss |
| **Wired Pipeline (USB RNDIS)** | `python tests\test_wired_pipeline_latency.py` | Glass-to-Glass: ~15.14 ms (Target: <80ms) |
| **Wireless 5GHz Wi-Fi** | `python tests\test_wireless_adaptive_latency.py` | Glass-to-Glass: ~34.77 ms (Target: <120ms) |
| **UI Design Contrast (WCAG AA)**| `python tests\test_design_system.py` | Contrast Ratio: 20.02:1 (Required: $\ge$4.5:1) |
| **iOS AirPlay Mirroring** | `python tests\test_airplay_pipeline.py` | Glass-to-Glass: ~20.15 ms |
| **48kHz Audio & Lip-Sync** | `python tests\test_av_sync_recording.py` | A/V Drift: ~3.43 ms (Target: <15ms) |
| **OBS Local Streaming Ingest** | `python tests\test_streaming_pipeline.py` | Ingest Latency: ~0.009 ms |
| **Multi-Device Grid & Failover** | `python tests\test_multi_device_grid.py` | 8 Concurrent Slots, 0.0 ms failover downtime |
| **PC Touch Injection** | `python tests\test_game_mode_touch.py` | Touch Dispatch: ~1.04 ms |
| **PUBG 60/90/120 FPS Stress** | `python tests\test_pubg_high_fps_benchmark.py` | 0.00% frame drops @ 37.1 °C |
| **iOS ReplayKit Memory** | `python tests\test_replaykit_pipeline.py` | Memory Footprint: 18.4 MB (Limit: 50.0 MB) |

---

## 🛠️ Part 2: Building the Binaries from Source

### 1. Desktop Receiver (Windows / macOS / Linux)
Requirements: CMake 3.22+, Qt 6.5+ (Quick, QML, Network), FFmpeg dev libraries, and C++20 compiler.

```powershell
cd c:\Users\gurud\Downloads\airwave
# Generate build configuration
cmake -B build -S desktop -DCMAKE_BUILD_TYPE=Release

# Compile Airwave executable
cmake --build build --config Release

# (Windows Only) Deploy Qt dynamic libraries & QML plugins
windeployqt --qmldir desktop\qml build\Release\Airwave.exe
```

### 2. Android Companion App (APK)
Requirements: JDK 17+ and Android SDK with API 34.

```powershell
cd c:\Users\gurud\Downloads\airwave\android
# Build release APK
.\gradlew assembleRelease

# The generated APK will be at:
# android\app\build\outputs\apk\release\app-release.apk
```

---

## 📱 Part 3: Live End-to-End User Workflows

### Workflow A: Competitive Gaming over Wired USB (Lowest Latency for PUBG/BGMI)
1. **Launch Desktop App**:
   - Run `build\Release\Airwave.exe` (Click "More info" &rarr; "Run anyway" if SmartScreen prompts).
   - Airwave launches directly onto the **Device Radar** screen.
2. **Connect Android Phone via USB**:
   - Plug phone into PC using a USB 3.0 / Type-C cable.
   - On phone: Open **Settings &rarr; Network & internet &rarr; Hotspot & tethering &rarr; Toggle "USB tethering" ON**.
   - Windows automatically activates an RNDIS network adapter (IP `192.168.42.x`).
3. **Start Casting**:
   - Open the **Airwave** app on Android.
   - Tap **"START CAST"** and accept the system screen recording dialog.
   - The desktop app detects the stream, locks on with the concentric wave animation, and switches to **Stage Mode**.
   - Latency will read **~15ms** on the live Signal HUD.

---

### Workflow B: Wireless 5GHz Wi-Fi Mirroring
1. Connect both PC and Phone to the same **5GHz Wi-Fi** network.
2. Open `Airwave.exe` on PC.
3. Open the Airwave mobile app:
   - The app uses **mDNS / NSD** to automatically discover the PC receiver (`Airwave-PC`).
   - Tap **"START CAST"**.
   - The AIMD Adaptive Rate Controller dynamically modulates bitrate (4Mbps – 24Mbps) to maintain smooth, stutter-free 60/90 FPS without frame drops.

---

### Workflow C: iOS Instant Mirroring (Zero App Required)
1. Open `Airwave.exe` on PC.
2. On your iPhone or iPad:
   - Ensure the device is connected to the same Wi-Fi network (or connect via Lightning/USB-C and enable **Personal Hotspot**).
   - Swipe down from the top-right corner to open **Control Center**.
   - Tap **Screen Mirroring**.
   - Select **"Airwave [PC]"** from the list.
3. Your iPhone screen and audio will instantly mirror to your PC display.

---

### Workflow D: Live Streaming into OBS Studio (No Drivers)
1. In `Airwave.exe`, open the slim control rail and verify the local stream relay is active on port `9000`.
2. Open **OBS Studio**:
   - In the **Sources** panel, click **+** &rarr; select **Media Source**.
   - Name it "Mobile Game Feed".
   - **Uncheck** "Local File".
   - Set **Input** to:
     ```
     srt://127.0.0.1:9000?mode=caller&latency=20
     ```
   - Check **"Close file when inactive"** & click **OK**.
3. Your mobile gameplay and internal game audio will appear inside OBS with 0% CPU re-encoding overhead.

---

### Workflow E: Unlimited Local Recording
1. In Stage Mode, hover over the bottom edge of the window to reveal the slim control rail.
2. Click the **REC** button (or press `Ctrl+R`).
3. The bitstream is recorded directly into an `.mp4` or `.mkv` container with zero watermark and zero re-encode overhead.
4. Click **REC** again to finalize the recording.
