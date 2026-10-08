# iOS Companion App Sideloading & ReplayKit Guide

## Overview
Airwave provides two ways to mirror your iPhone or iPad:
1. **AirPlay Receiver (Mode A - Recommended)**: Zero apps needed. Works straight out of the box via Control Center Screen Mirroring.
2. **ReplayKit Companion App (Mode B - Optional)**: Provides in-game broadcast extension with custom bitrate and direct UDP transport.

---

## ⚠️ Free Apple Developer Sideloading Constraints (Zero-Budget)
Apple imposes strict rules on apps sideloaded with a free Personal Apple ID:
- **7-Day Expiration**: Sideloaded apps expire after 7 days. You must re-sign using AltStore, SideStore, or Sideloadly.
- **3-App Active Limit**: A free account can have at most 3 active sideloaded apps at once.
- **ReplayKit 50MB Memory Limit**: iOS forcefully kills any Broadcast Upload Extension consuming over 50MB of RAM. Airwave's extension uses direct `CVPixelBuffer` -> `VTCompressionSession` pipelines to maintain a lean ~18MB footprint.

---

## Sideloading Instructions

### Method 1: AltStore / SideStore
1. Install [AltStore](https://altstore.io/) on your PC/Mac.
2. Download `Airwave-iOS.ipa` from the latest [GitHub Release](https://github.com/airwave/airwave/releases).
3. In AltStore on your iPhone, tap **+** -> select `Airwave-iOS.ipa`.
4. Launch Airwave, tap **"Enable Game Broadcast"**, and select Airwave from the iOS Broadcast picker.

### Method 2: Sideloadly
1. Connect iPhone via Lightning / USB-C cable to your PC.
2. Open Sideloadly, drag and drop `Airwave-iOS.ipa`.
3. Enter your Apple ID and click **Start**.
4. On iPhone, navigate to **Settings -> General -> VPN & Device Management** -> Trust your Developer profile.
