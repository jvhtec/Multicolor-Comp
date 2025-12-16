# Multi-Color Comp - Compiled Binaries

This directory contains pre-compiled plugin binaries for Multi-Color Comp.

## Included Formats

### VST3 (Linux x86_64)
- **File:** `Multi-Color Comp.vst3/`
- **Size:** 4.0 MB
- **Platform:** Linux x86_64
- **Build:** Release with optimizations

### macOS Builds (VST3 + AU + AAX)
To build for macOS, compile on a Mac:

```bash
# Quick build (VST3 + AU)
./build_macos.sh

# Build with AAX (requires AAX SDK from Avid)
export AAX_SDK_PATH=~/SDKs/AAX_SDK
./build_macos.sh
```

**macOS Output:**
- **VST3:** Universal binary (Apple Silicon + Intel)
- **AU:** Audio Unit (macOS native format)
- **AAX:** Pro Tools format (requires AAX SDK + code signing)

### Windows Builds (VST3 + AAX)
To build for Windows:

```cmd
REM Quick build (VST3 + Standalone)
build_windows.bat

REM Build with AAX (requires AAX SDK from Avid)
set AAX_SDK_PATH=C:\SDKs\AAX_SDK
build_windows.bat
```

**Windows Output:**
- **VST3:** Windows x64 plugin
- **Standalone:** Standalone .exe application
- **AAX:** Pro Tools format (requires AAX SDK + PACE code signing)

**Cross-compile from Linux:**
```bash
./build_windows.sh  # Requires MinGW-w64
```

See [BUILD_INSTRUCTIONS.md](../BUILD_INSTRUCTIONS.md) for detailed instructions.

## Installation

### Linux VST3
```bash
# Copy to user directory
cp -r "Multi-Color Comp.vst3" ~/.vst3/

# Or system-wide
sudo cp -r "Multi-Color Comp.vst3" /usr/lib/vst3/
```

### macOS VST3
```bash
cp -r "Multi-Color Comp.vst3" ~/Library/Audio/Plug-Ins/VST3/
```

### macOS Audio Unit
```bash
cp -r "Multi-Color Comp.component" ~/Library/Audio/Plug-Ins/Components/
```

### macOS AAX (Pro Tools)
```bash
sudo cp -r "Multi-Color Comp.aaxplugin" /Library/Application\ Support/Avid/Audio/Plug-Ins/
```

### Windows VST3
```cmd
xcopy /E /I "Multi-Color Comp.vst3" "%COMMONPROGRAMFILES%\VST3\Multi-Color Comp.vst3"
```

Or manually copy to:
- System: `C:\Program Files\Common Files\VST3\`
- User: `C:\Users\<username>\AppData\Local\Programs\Common\VST3\`

### Windows AAX (Pro Tools)
```cmd
xcopy /E /I "Multi-Color Comp.aaxplugin" "%COMMONPROGRAMFILES%\Avid\Audio\Plug-Ins\Multi-Color Comp.aaxplugin"
```

**Note:** AAX plugins must be signed to run in Pro Tools.

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

### Quick Start

**macOS:**
```bash
cd ..
./build_macos.sh
```

**Linux:**
```bash
cd ..
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel $(nproc)
```

**Windows:**
```cmd
cd ..
mkdir build && cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release --parallel
```

### AAX Build (All Platforms)

AAX format requires:
1. **Avid AAX SDK** - Register at https://www.avid.com/alliance-partner-program
2. **Code signing** - Required for Pro Tools compatibility

```bash
# Set SDK path
export AAX_SDK_PATH=/path/to/AAX_SDK  # macOS/Linux
set AAX_SDK_PATH=C:\path\to\AAX_SDK    # Windows

# Build
./build_macos.sh  # macOS
# (or use cmake commands for other platforms)

# Sign with PACE wraptool (included in AAX SDK)
wraptool sign --account "your@email.com" --password "pass" ...
```

For complete build instructions, see [BUILD_INSTRUCTIONS.md](../BUILD_INSTRUCTIONS.md).

## System Requirements

- Linux x86_64
- VST3-compatible DAW (Reaper, Bitwig, Ardour, etc.)
- SSE2 instruction set (standard on modern x86_64)

## Support

For issues or questions, please refer to the main repository README.
