# Multi-Color Comp - Compiled Binaries

This directory contains pre-compiled plugin binaries for Multi-Color Comp.

## Included Formats

### VST3 (Linux x86_64)
- **File:** `Multi-Color Comp.vst3/`
- **Size:** 4.0 MB
- **Platform:** Linux x86_64
- **Build:** Release with optimizations

## Installation

### Linux VST3
```bash
# Copy to system VST3 directory
sudo cp -r "Multi-Color Comp.vst3" ~/.vst3/

# Or copy to user directory
cp -r "Multi-Color Comp.vst3" ~/.vst3/
```

## Plugin Information

- **Framework:** JUCE 7.0.12
- **C++ Standard:** C++20
- **Build Date:** December 16, 2025
- **Git Commit:** See parent directory for source

## Features

- Multi-style compressor (VCA/FET/Opto/Vari-Mu)
- Harmonic color module with 4 saturation types
- FFT-based resonance control (Soothe)
- Intensity macro for quick workflow
- Flexible signal routing
- Modern dark UI with real-time metering

## Building from Source

To build other formats (AAX, macOS, Windows):

```bash
cd ..
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

**Note:** AAX format requires Avid AAX SDK (not included in repository).

## System Requirements

- Linux x86_64
- VST3-compatible DAW (Reaper, Bitwig, Ardour, etc.)
- SSE2 instruction set (standard on modern x86_64)

## Support

For issues or questions, please refer to the main repository README.
