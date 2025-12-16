# Build Instructions

This document provides detailed instructions for building Multi-Color Comp on different platforms and formats.

## Prerequisites

### All Platforms
- CMake 3.22 or higher
- C++20 compatible compiler
- Git (for fetching JUCE)

### macOS
- Xcode 12.0 or higher
- Xcode Command Line Tools: `xcode-select --install`
- CMake: `brew install cmake`

### Windows
- Visual Studio 2019 or higher
- CMake: Download from https://cmake.org/download/

### Linux
- GCC 9+ or Clang 10+
- Build essentials: `sudo apt install build-essential cmake`
- Required libraries: `sudo apt install libasound2-dev libcurl4-openssl-dev libfreetype6-dev libx11-dev libxcomposite-dev libxcursor-dev libxinerama-dev libxrandr-dev mesa-common-dev`

## Building on macOS

### Quick Build (VST3 + AU)

```bash
./build_macos.sh
```

This creates a universal binary (Apple Silicon + Intel) with VST3 and AU formats.

### Custom Architecture

For Apple Silicon only:
```bash
MACOS_ARCHITECTURES=arm64 ./build_macos.sh
```

For Intel only:
```bash
MACOS_ARCHITECTURES=x86_64 ./build_macos.sh
```

### Building with AAX

1. **Obtain AAX SDK**
   - Register as a developer at https://www.avid.com/alliance-partner-program
   - Download the AAX SDK
   - Extract to a location (e.g., `~/SDKs/AAX_SDK`)

2. **Set AAX SDK path and build**
   ```bash
   export AAX_SDK_PATH=~/SDKs/AAX_SDK
   ./build_macos.sh
   ```

3. **Code Signing for AAX**

   AAX plugins must be signed to run in Pro Tools. After building:

   ```bash
   # Sign with Avid's pace tool (requires AAX developer account)
   wraptool sign \
       --verbose \
       --account "your-account@email.com" \
       --password "your-password" \
       --wcguid "your-wcguid" \
       --keyfile "your-key.p12" \
       --keypassword "key-password" \
       --in "build_macos/MultiColorComp_artefacts/Release/AAX/Multi-Color Comp.aaxplugin" \
       --out "Multi-Color Comp-signed.aaxplugin"
   ```

### Manual Build

```bash
mkdir build_macos && cd build_macos

# Configure
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
    -DAAX_SDK_PATH=~/SDKs/AAX_SDK  # Optional, for AAX

# Build
cmake --build . --config Release --parallel 8

# Built plugins are in:
# - build_macos/MultiColorComp_artefacts/Release/VST3/
# - build_macos/MultiColorComp_artefacts/Release/AU/
# - build_macos/MultiColorComp_artefacts/Release/AAX/ (if AAX SDK provided)
```

## Building on Windows

### Quick Build (VST3)

Using the provided build script:

```cmd
build_windows.bat
```

This automatically detects Visual Studio and builds VST3 + Standalone formats.

### Quick Build with AAX

```cmd
set AAX_SDK_PATH=C:\SDKs\AAX_SDK
build_windows.bat
```

### Manual Build - Visual Studio 2022

```cmd
mkdir build_windows
cd build_windows

REM Configure
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release

REM Build
cmake --build . --config Release --parallel

REM Built plugins are in:
REM build_windows\MultiColorComp_artefacts\Release\VST3\Multi-Color Comp.vst3
REM build_windows\MultiColorComp_artefacts\Release\Standalone\Multi-Color Comp.exe
```

### Manual Build - Visual Studio 2019

```cmd
mkdir build_windows
cd build_windows

REM Configure
cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release

REM Build
cmake --build . --config Release --parallel
```

### AAX Build on Windows

1. **Set AAX SDK path**
   ```cmd
   set AAX_SDK_PATH=C:\SDKs\AAX_SDK
   ```

2. **Build**
   ```cmd
   build_windows.bat
   ```

   Or manually:
   ```cmd
   mkdir build_windows
   cd build_windows
   cmake .. -G "Visual Studio 17 2022" -A x64 -DAAX_SDK_PATH=%AAX_SDK_PATH%
   cmake --build . --config Release --parallel
   ```

3. **Sign the AAX plugin (required for Pro Tools)**

   AAX plugins must be signed using PACE wraptool (included in AAX SDK):

   ```cmd
   cd build_windows\MultiColorComp_artefacts\Release\AAX

   wraptool sign ^
       --verbose ^
       --account "your-account@email.com" ^
       --password "your-password" ^
       --wcguid "your-wcguid" ^
       --keyfile "path\to\your-key.p12" ^
       --keypassword "key-password" ^
       --in "Multi-Color Comp.aaxplugin" ^
       --out "Multi-Color Comp-signed.aaxplugin"
   ```

### Cross-Compilation from Linux

For Linux users who want to build Windows binaries:

```bash
# Install MinGW (Ubuntu/Debian)
sudo apt install mingw-w64 cmake

# Build
./build_windows.sh
```

This creates Windows x64 binaries using MinGW-w64 cross-compiler.

## Building on Linux

```bash
mkdir build_linux && cd build_linux

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release --parallel $(nproc)

# Built plugin is in:
# build_linux/MultiColorComp_artefacts/Release/VST3/Multi-Color Comp.vst3
```

## Installation

### macOS
```bash
# VST3
cp -r "Multi-Color Comp.vst3" ~/Library/Audio/Plug-Ins/VST3/

# Audio Unit
cp -r "Multi-Color Comp.component" ~/Library/Audio/Plug-Ins/Components/

# AAX (requires system permissions)
sudo cp -r "Multi-Color Comp.aaxplugin" /Library/Application\ Support/Avid/Audio/Plug-Ins/
```

### Windows
```cmd
REM VST3
copy "Multi-Color Comp.vst3" "%COMMONPROGRAMFILES%\VST3\"

REM AAX
copy "Multi-Color Comp.aaxplugin" "%COMMONPROGRAMFILES%\Avid\Audio\Plug-Ins\"
```

### Linux
```bash
# VST3
cp -r "Multi-Color Comp.vst3" ~/.vst3/

# Or system-wide
sudo cp -r "Multi-Color Comp.vst3" /usr/lib/vst3/
```

## Build Configurations

### Release (Default)
Optimized for performance:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### Debug
With debug symbols:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

### RelWithDebInfo
Optimized with debug symbols (good for profiling):
```bash
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

## Troubleshooting

### macOS: "Plugin not trusted"
Sign the plugin or disable Gatekeeper temporarily:
```bash
sudo spctl --master-disable  # Disable
# Test plugin
sudo spctl --master-enable   # Re-enable
```

Or add exception:
```bash
xattr -dr com.apple.quarantine "Multi-Color Comp.vst3"
```

### AAX: "Plugin not authorized"
AAX plugins must be signed with PACE wraptool using valid Avid developer credentials.

### Linux: Missing dependencies
```bash
sudo apt install \
    libasound2-dev \
    libcurl4-openssl-dev \
    libfreetype6-dev \
    libx11-dev \
    libxcomposite-dev \
    libxcursor-dev \
    libxinerama-dev \
    libxrandr-dev \
    mesa-common-dev
```

### JUCE fetch fails
If JUCE download fails, manually clone:
```bash
git clone --depth 1 --branch 7.0.12 https://github.com/juce-framework/JUCE.git _deps/juce-src
```

## AAX SDK Setup

### Obtaining AAX SDK

1. Visit https://www.avid.com/alliance-partner-program
2. Apply for developer account (may take several days)
3. Download AAX SDK from developer portal
4. Extract to preferred location

### Directory Structure

The AAX SDK should have this structure:
```
AAX_SDK/
├── Interfaces/
├── Extensions/
├── Libs/
└── ...
```

### Code Signing Requirements

AAX plugins require:
- Valid Avid developer account
- PACE wraptool (included in AAX SDK)
- Code signing certificate from Avid
- Signed plugins to run in Pro Tools

## CI/CD

For automated builds, see `.github/workflows/` (if configured).

## Additional Resources

- JUCE Documentation: https://docs.juce.com/
- AAX SDK Documentation: https://developer.avid.com/
- CMake Documentation: https://cmake.org/documentation/
- Plugin format specs: Check respective vendor documentation
