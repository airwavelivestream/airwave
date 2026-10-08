# Contributing to Airwave

Thank you for helping make Airwave the fastest, cleanest, 100% free screen mirroring and mobile gaming broadcast tool!

## Core Tenets
1. **100% Free Forever**: Zero paywalls, zero watermarks, zero time/device limits.
2. **Performance First**: Any code adding CPU overhead to the decode or render pipeline must be benchmarked.
3. **No Proprietary Drivers**: Features must rely on open native standards (SRT, Media Foundation, RNDIS, AirPlay).

## Development Setup

### Desktop (C++20 & Qt 6)
```bash
# 1. Clone repository
git clone https://github.com/airwave/airwave.git
cd airwave

# 2. Build desktop receiver with CMake
cmake -B build -S desktop -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### Android (Kotlin & Jetpack Compose)
```bash
cd android
./gradlew assembleDebug
```

## Running Latency Tests & Benchmarks
```bash
python tests/test_loopback_latency.py
python tests/test_wired_pipeline_latency.py
python tests/test_pubg_high_fps_benchmark.py
```

## Licensing
By contributing, you agree that your contributions will be licensed under the **GNU General Public License v3.0 (GPL-3.0)**.
