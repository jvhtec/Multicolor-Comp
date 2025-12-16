#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "Smoothing.h"
#include "../Parameters.h"
#include <vector>
#include <complex>

/**
 * FFT-based adaptive resonance control
 * Detects and attenuates harsh resonances
 */
class SootheModule
{
public:
    SootheModule();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void process(juce::dsp::AudioBlock<float>& block, const Parameters& params);

    int getLatencySamples() const { return latencySamples; }

    enum Quality
    {
        Eco = 0,
        Normal,
        High
    };

private:
    struct ChannelState
    {
        // FFT buffers
        std::vector<float> inputFIFO;
        std::vector<float> outputFIFO;
        std::vector<float> fftData;
        std::vector<float> window;
        std::vector<float> ifftData;

        // Spectral processing
        std::vector<float> magnitudes;
        std::vector<float> baseline;
        std::vector<float> attenuation;
        std::vector<float> resonanceScore;

        // Overlap-add
        std::vector<float> overlapBuffer;

        int fifoIndex = 0;
        int hopCounter = 0;

        void reset()
        {
            std::fill(inputFIFO.begin(), inputFIFO.end(), 0.0f);
            std::fill(outputFIFO.begin(), outputFIFO.end(), 0.0f);
            std::fill(fftData.begin(), fftData.end(), 0.0f);
            std::fill(overlapBuffer.begin(), overlapBuffer.end(), 0.0f);
            std::fill(attenuation.begin(), attenuation.end(), 1.0f);
            fifoIndex = 0;
            hopCounter = 0;
        }
    };

    std::unique_ptr<juce::dsp::FFT> fft;
    std::array<ChannelState, 2> channelState;

    double sampleRate = 44100.0;
    int fftSize = 2048;
    int hopSize = 512;
    int latencySamples = 0;
    Quality currentQuality = Quality::Normal;

    // Processing
    void processFFTFrame(ChannelState& state, const Parameters& params);
    void computeBaseline(ChannelState& state, float smoothingWidth);
    void computeResonanceScore(ChannelState& state, float sensitivity, float focusLow, float focusHigh);
    void updateAttenuation(ChannelState& state, float amount, float speed, float sharpness);
    void applyAttenuation(ChannelState& state);

    // Helpers
    void createWindow(std::vector<float>& window, int size);
    void smoothSpectrum(const std::vector<float>& input, std::vector<float>& output, int width);
    int freqToFFTBin(float freq) const;
};
