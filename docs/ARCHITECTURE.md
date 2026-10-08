# Airwave Architecture, Protocol Specification & Risk Analysis

## 1. System Overview & Scope
Airwave is a high-performance, low-latency, zero-cost phone-to-PC screen mirroring and game streaming suite.
- **Desktop Receiver**: C++20, Qt 6 (QML), FFmpeg Hardware Decode (D3D11VA/NVDEC/QuickSync/AMF/VideoToolbox), Qt RHI Direct Rendering.
- **Android Sender**: Kotlin, MediaProjection, asynchronous MediaCodec with low-latency zero-latency buffer flags, Android 10+ AudioPlaybackCapture.
- **iOS Receiver**: Built-in AirPlay mirroring receiver (GPL-3.0 compatible adaptation from UxPlay) + optional sideloaded ReplayKit extension.
- **Transport Layer**: Custom Framing Protocol over UDP (Media packets with FEC & lightweight NACK) + TCP (Reliable Control & Session Handshake), adaptive rate control, fallback to Wi-Fi from USB tethering without dropping stream.

---

## 2. License Decision: GPL-3.0
- **AirPlay Component**: UxPlay is licensed under GPL-3.0. Linking or bundling it requires the desktop receiver application and the unified repository to be licensed under **GNU General Public License v3.0 (GPL-3.0)**.
- **Scrcpy references**: Scrcpy is Apache 2.0 (permissively reusable within GPL-3.0).
- **Qt 6**: Dynamic linking under LGPLv3 (strictly compatible with GPL-3.0).
- **FFmpeg**: Configured with `--enable-gpl`.

---

## 3. High-Performance Framing Protocol (AWTP - Airwave Transport Protocol)

### Packet Wire Format (Binary Big-Endian)
```
+---------------------------------------------------------------+
| AWTP Header (16 Bytes)                                        |
+---------------+---------------+---------------+---------------+
| Magic (2B)    | Version (1B)  | StreamID (1B) | MsgType (1B)  |
| 0x41 0x57     | 0x01          | 0x00..0xFF    | Packet Type   |
+---------------+---------------+---------------+---------------+
| Flags (1B)    | Sequence Number (2B)          | Timestamp (4B)|
| [Key|EOS|FEC] | Incremental Packet Seq        | Microseconds  |
+---------------+---------------+---------------+---------------+
| Payload Length (2B)           | Checksum / Reserved (4B)      |
+---------------------------------------------------------------+
| Payload Data (NAL units, AAC/Opus frames, Control JSON)       |
| ...                                                           |
+---------------------------------------------------------------+
```

#### MsgType Definitions
- `0x01`: Control / Handshake (TCP or UDP)
- `0x02`: Video NAL Unit Fragment (UDP)
- `0x03`: Audio Frame (UDP)
- `0x04`: Feedback / NACK / Stats (UDP)
- `0x05`: Ping / Latency probe (UDP)

---

## 4. Hardware Acceleration & Glass-to-Glass Budget

Target: Glass-to-Glass Latency < 80ms (Wired), < 120ms (5GHz Wi-Fi)

| Pipeline Stage | Android (Snapdragon 8 Gen 1+) | PC (D3D11 / NVDEC / 144Hz) | Target Budget |
| :--- | :--- | :--- | :--- |
| Screen Capture | MediaProjection (Direct Surface) | - | 4 - 8 ms |
| HW Encoder | MediaCodec (H.264 Baseline/Constrained High, CBR, low-latency, 0 B-frames, I-frame rate 1s) | - | 8 - 14 ms |
| Packetize & Net | AWTP UDP socket send | USB Tether (RNDIS/NCM) / 5GHz Wi-Fi | 2 - 6 ms (wired), 8 - 18 ms (Wi-Fi) |
| Jitter Buffer | - | Lock-free 1-2 frame ring buffer | 0 - 8 ms |
| HW Decoder | - | FFmpeg D3D11VA / DXVA2 / NVDEC | 4 - 9 ms |
| Rendering | - | Qt RHI D3D11 SwapChain / Flip model | 4 - 8 ms |
| Total Budget | | | **22 - 53 ms (Wired)** / **35 - 75 ms (Wi-Fi)** |

---

## 5. Risk Matrix & Mitigations

| Risk / Limitation | Root Cause | Impact | Mitigation Strategy |
| :--- | :--- | :--- | :--- |
| **Android Audio Capture Block** | Apps setting `FLAG_SECURE` or `USAGE_GAME` opting out of `AudioPlaybackCapture` | No game sound in casting | Clear UI warning; fallback to microphone mix or USB accessory audio forwarding; document PUBG/BGMI in-game audio capture permissions. |
| **OEM Background App Killers** | MIUI, ColorOS, EMUI aggressively terminate Foreground Services | Stream drops abruptly | In-app battery optimization exemption prompt (`REQUEST_IGNORE_BATTERY_OPTIMIZATIONS`), sticky notification, autostart checklist. |
| **iOS ReplayKit 50MB Limit** | Apple limits Broadcast Upload Extensions to 50MB RAM | Extension crash on high bitrate | Minimal native memory footprint, stream directly via VideoToolbox zero-copy pixel buffers, or use zero-install **AirPlay Receiver** mode. |
| **Windows SmartScreen / Unsigned** | Unsigned binary warning on Windows 10/11 | User drop-off | In-app & documentation prompt: click "More Info" -> "Run anyway"; publish verifiable SHA256 checksums on GitHub Releases. |
| **macOS Gatekeeper / Quarantine** | macOS blocks unsigned executables | "App is damaged and cannot be opened" | Document `xattr -cr /Applications/Airwave.app` and right-click -> Open flow. |
| **Wi-Fi Packet Loss / Jitter** | 2.4GHz interference or crowded 5GHz channels | Stutter, keyframe freezes | Adaptive bitrate algorithm (AIMD based on RTT & loss), lightweight NACK + intra-refresh slice recovery without full IDR stalls. |
