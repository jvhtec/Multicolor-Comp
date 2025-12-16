#!/bin/bash

# Multi-Color Comp - Windows Cross-Compilation Script (Linux)
# This script cross-compiles for Windows on Linux using MinGW

set -e

echo "🎛️  Cross-compiling Multi-Color Comp for Windows..."
echo ""

# Check if we're on Linux
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo "❌ Error: This cross-compilation script is for Linux only"
    echo "   On Windows, use build_windows.bat instead"
    exit 1
fi

# Check for MinGW
if ! command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    echo "❌ Error: MinGW-w64 not found."
    echo ""
    echo "Install with:"
    echo "  Ubuntu/Debian: sudo apt install mingw-w64 cmake"
    echo "  Fedora:        sudo dnf install mingw64-gcc-c++ cmake"
    echo "  Arch:          sudo pacman -S mingw-w64-gcc cmake"
    exit 1
fi

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "❌ Error: CMake not found. Install with your package manager."
    exit 1
fi

# Configuration
BUILD_DIR="build_windows_cross"
BUILD_TYPE="${CMAKE_BUILD_TYPE:-Release}"
MINGW_PREFIX="x86_64-w64-mingw32"

echo "📋 Build Configuration:"
echo "   Build Type: $BUILD_TYPE"
echo "   Target: Windows x64"
echo "   Toolchain: MinGW-w64"

# Check for AAX SDK
if [ -n "$AAX_SDK_PATH" ] && [ -d "$AAX_SDK_PATH" ]; then
    echo "   AAX SDK: Found at $AAX_SDK_PATH"
    AAX_CMAKE_FLAG="-DAAX_SDK_PATH=$AAX_SDK_PATH"
else
    echo "   AAX SDK: Not found (set AAX_SDK_PATH to enable)"
    AAX_CMAKE_FLAG=""
fi
echo ""

# Create toolchain file
TOOLCHAIN_FILE="$BUILD_DIR/mingw-w64-toolchain.cmake"
mkdir -p "$BUILD_DIR"

cat > "$TOOLCHAIN_FILE" << 'EOF'
# MinGW-w64 toolchain file for cross-compilation
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Specify the cross compiler
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# Where to find target libraries
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)

# Search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# For libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Enable static linking to avoid DLL dependencies
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
set(CMAKE_SHARED_LINKER_FLAGS "-static-libgcc -static-libstdc++")
EOF

echo "⚙️  Configuring CMake for Windows cross-compilation..."
cd "$BUILD_DIR"

cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="mingw-w64-toolchain.cmake" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    $AAX_CMAKE_FLAG

echo ""
echo "🔨 Building plugins..."
cmake --build . --config "$BUILD_TYPE" --parallel $(nproc)

echo ""
echo "✅ Build complete!"
echo ""
echo "📦 Built plugins are located at:"
echo "   VST3: $BUILD_DIR/MultiColorComp_artefacts/$BUILD_TYPE/VST3/Multi-Color Comp.vst3"
echo "   Standalone: $BUILD_DIR/MultiColorComp_artefacts/$BUILD_TYPE/Standalone/Multi-Color Comp.exe"

if [ -n "$AAX_CMAKE_FLAG" ]; then
    echo "   AAX:  $BUILD_DIR/MultiColorComp_artefacts/$BUILD_TYPE/AAX/Multi-Color Comp.aaxplugin"
fi

echo ""
echo "📥 To deploy on Windows:"
echo "   Copy VST3 folder to: C:\\Program Files\\Common Files\\VST3\\"

if [ -n "$AAX_CMAKE_FLAG" ]; then
    echo "   Copy AAX to: C:\\Program Files\\Common Files\\Avid\\Audio\\Plug-Ins\\"
    echo "   (AAX must be signed with PACE wraptool)"
fi

echo ""
echo "🎉 Done!"
