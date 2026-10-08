# Airwave Comprehensive Troubleshooting Guide

## 1. Android Wired (USB Tethering) Issues

### "PC doesn't detect phone after plugging USB cable"
- **Cause**: Android default USB mode is usually "Charging only".
- **Fix**:
  1. Open Android **Settings -> Network & internet -> Hotspot & tethering**.
  2. Toggle **USB tethering** to **ON**.
  3. Windows will immediately detect a new `Remote NDIS Compatible Device` network interface (assigned IP `192.168.42.x`).
  4. Launch Airwave; the Device Radar will automatically discover the phone.

### "No game audio in PUBG / BGMI"
- **Cause**: Android 10+ limits apps from capturing audio if another app sets strict security policies.
- **Fix**:
  1. Verify the companion app was granted the Audio Recording permission.
  2. Ensure your phone volume is not muted.
  3. If your OEM blocks `AudioPlaybackCapture` for specific games, toggle **Microphone Mix** mode in Airwave companion settings.

### "App gets closed or freezes during long gaming sessions"
- **Cause**: OEM aggressive battery killer (MIUI Battery Saver / ColorOS Background Freeze).
- **Fix**:
  1. Open Android **Settings -> Apps -> Airwave -> Battery -> Set to 'Unrestricted'**.
  2. Lock the Airwave app in your recent apps tray.

---

## 2. Windows Desktop Issues

### "Windows SmartScreen prevented an unrecognized app from starting"
- **Cause**: Airwave is 100% free and open-source, avoiding costly $400/yr enterprise signing certificates.
- **Fix**:
  1. Click **More info**.
  2. Click **Run anyway**.

### "Black Screen with Audio on Desktop"
- **Cause**: Missing Direct3D 11 hardware decoder support or outdated GPU drivers.
- **Fix**:
  1. Update your Intel / NVIDIA / AMD graphics drivers.
  2. In Airwave Settings, switch Hardware Decoder from `D3D11VA` to `DXVA2` or `Software (CPU)`.

---

## 3. iOS Screen Mirroring Issues

### "Airwave does not show up in Control Center Screen Mirroring"
- **Cause**: PC and iPhone are not on the same Wi-Fi subnet, or Windows Firewall is blocking Bonjour.
- **Fix**:
  1. Ensure both devices are connected to the same 5GHz Wi-Fi band (avoid Guest Wi-Fi networks).
  2. Allow `Airwave.exe` through Windows Defender Firewall for Private Networks.
  3. For zero Wi-Fi latency, plug iPhone via Lightning/USB-C, turn on **Personal Hotspot**, and cast over USB.
