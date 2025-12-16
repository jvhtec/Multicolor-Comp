#!/bin/bash

# Multi-Color Comp - macOS Build Script
# This script builds VST3, AU, and optionally AAX formats on macOS

set -e

echo "🎛️  Building Multi-Color Comp for macOS..."
echo ""

# Check if we're on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "❌ Error: This script must be run on macOS"
    exit 1
fi

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "❌ Error: CMake not found. Install with: brew install cmake"
    exit 1
fi

# Configuration
BUILD_DIR="build_macos"
BUILD_TYPE="${CMAKE_BUILD_TYPE:-Release}"
ARCHITECTURES="${MACOS_ARCHITECTURES:-arm64;x86_64}"  # Universal binary by default

echo "📋 Build Configuration:"
echo "   Build Type: $BUILD_TYPE"
echo "   Architectures: $ARCHITECTURES"

# Check for AAX SDK
if [ -n "$AAX_SDK_PATH" ] && [ -d "$AAX_SDK_PATH" ]; then
    echo "   AAX SDK: Found at $AAX_SDK_PATH"
    AAX_CMAKE_FLAG="-DAAX_SDK_PATH=$AAX_SDK_PATH"
else
    echo "   AAX SDK: Not found (set AAX_SDK_PATH to enable)"
    AAX_CMAKE_FLAG=""
fi
echo ""

# Create and enter build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure
echo "⚙️  Configuring CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_OSX_ARCHITECTURES="$ARCHITECTURES" \
    $AAX_CMAKE_FLAG

# Build
echo ""
echo "🔨 Building plugins..."
cmake --build . --config "$BUILD_TYPE" --parallel $(sysctl -n hw.ncpu)

echo ""
echo "✅ Build complete!"
echo ""
echo "📦 Built plugins are located at:"
echo "   VST3: $BUILD_DIR/MultiColorComp_artefacts/$BUILD_TYPE/VST3/Multi-Color Comp.vst3"
echo "   AU:   $BUILD_DIR/MultiColorComp_artefacts/$BUILD_TYPE/AU/Multi-Color Comp.component"

if [ -n "$AAX_CMAKE_FLAG" ]; then
    echo "   AAX:  $BUILD_DIR/MultiColorComp_artefacts/$BUILD_TYPE/AAX/Multi-Color Comp.aaxplugin"
fi

echo ""
echo "📥 To install:"
echo "   VST3: cp -r \"$BUILD_DIR/MultiColorComp_artefacts/$BUILD_TYPE/VST3/Multi-Color Comp.vst3\" ~/Library/Audio/Plug-Ins/VST3/"
echo "   AU:   cp -r \"$BUILD_DIR/MultiColorComp_artefacts/$BUILD_TYPE/AU/Multi-Color Comp.component\" ~/Library/Audio/Plug-Ins/Components/"

if [ -n "$AAX_CMAKE_FLAG" ]; then
    echo "   AAX:  cp -r \"$BUILD_DIR/MultiColorComp_artefacts/$BUILD_TYPE/AAX/Multi-Color Comp.aaxplugin\" /Library/Application\\ Support/Avid/Audio/Plug-Ins/"
fi

echo ""
echo "🎉 Done!"
