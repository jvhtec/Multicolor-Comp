# Multi-Color Comp

A professional VST3/AAX plugin combining compression, harmonic saturation, and adaptive resonance control.

## Features

- **Compressor**: VCA, FET, Opto, and Vari-Mu styles
- **Color**: Tape, Tube, Transformer, and Clip saturation with oversampling
- **Soothe**: FFT-based adaptive resonance control
- **Flexible routing**: Process in two different signal paths
- **Quality modes**: Eco/Normal/High for CPU management

## Build Requirements

- CMake 3.22+
- C++20 compiler (GCC, Clang, MSVC)
- JUCE 7.0.12 (fetched automatically)

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

The plugin will be built in `build/MultiColorComp_artefacts/`.

## Testing

```bash
cd build
ctest --output-on-failure
```

Or run the test executable directly:
```bash
./tests/CompressorTest
```

## Development Status

### Week 1: Project skeleton ✓
- [x] JUCE plugin project structure
- [x] Parameter system with smoothing
- [x] Basic I/O and metering
- [x] Headless DSP tests

### Week 2-3: Core compressor ✓
- [x] VCA style implementation
- [x] Detector, envelope, gain computer
- [x] Soft knee
- [x] Stereo linking
- [x] Sidechain HPF

### Week 4: Style system ✓
- [x] FET, Opto, Vari-Mu modes
- [x] Style switching with crossfade
- [x] Parameter snapshots per style

### Week 5-6: Color module ✓
- [x] Oversampling engine (2x/4x/8x)
- [x] Tape, Tube, Transformer, Clip models
- [x] DC blocker
- [x] Drive and tone controls

### Week 7-8: Soothe module ✓
- [x] FFT-based spectral processing
- [x] Resonance detection
- [x] Adaptive attenuation
- [x] Focus range control
- [x] Delta monitoring
- [ ] Quality modes (Eco/Normal/High)

### Week 9: Routing & macros ✓
- [x] Routing toggle
- [x] Input/output trim
- [ ] Intensity macro
- [x] Per-module bypass

### Week 10-12: Polish & Release
- [ ] Full UI with meters
- [ ] Preset browser
- [ ] AAX build
- [ ] Installer & signing

## Architecture

```
Input → [Input Trim] → [Router] → [Output Trim] → Output
                          │
                          ├─ Route A: Soothe → Comp → Color
                          └─ Route B: Comp → Color → Soothe
```

Each module has:
- Bypass with smooth switching
- Parallel mix control
- Independent parameter smoothing

## DSP Details

### VCA Compressor
- Peak/RMS hybrid detector (30% peak, 70% RMS)
- Soft knee with quadratic transition
- Sidechain HPF at 80 Hz default
- Attack: 0.1-50 ms, Release: 10-1000 ms

### Color Types
- **Tape**: Soft tanh saturation with HF rolloff
- **Tube**: Even-harmonic emphasis
- **Transformer**: Odd-harmonic presence
- **Clip**: Hard clipping with controlled aliasing

### Soothe
- FFT size: 2048 (Normal quality)
- Hop size: 512 samples
- Baseline: Moving average smoothing
- Resonance score: Ratio-based with selectivity curve
- Max attenuation: -12 dB

## License

(Add your license here)
