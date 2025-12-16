#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "Smoothing.h"
#include "../Parameters.h"

/**
 * Production-quality compressor with multiple styles
 * VCA: Clean, controlled, peak/RMS hybrid
 * FET: Fast, aggressive
 * Opto: Smooth, program-dependent
 * Vari-Mu: Soft knee, gentle saturation
 */
class CompressorModule
{
public:
    CompressorModule();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void process(juce::dsp::AudioBlock<float>& block, const Parameters& params);

    float getGainReduction() const { return currentGR; }

    enum Style
    {
        VCA = 0,
        FET,
        Opto,
        VariMu
    };

private:
    struct CompressorState
    {
        // Detector envelope
        float envelopeRMS = 0.0f;
        float envelopePeak = 0.0f;
        float envelopeDB = -100.0f;

        // Gain reduction
        float gainLinear = 1.0f;

        // Sidechain HPF state (simple one-pole)
        float hpfState = 0.0f;

        void reset()
        {
            envelopeRMS = 0.0f;
            envelopePeak = 0.0f;
            envelopeDB = -100.0f;
            gainLinear = 1.0f;
            hpfState = 0.0f;
        }
    };

    // Per-channel state
    CompressorState state[2];

    // Smoothers
    ParameterSmoother attackSmoother;
    ParameterSmoother releaseSmoother;
    ParameterSmoother thresholdSmoother;
    ParameterSmoother ratioSmoother;
    ParameterSmoother kneeSmoother;
    ParameterSmoother makeupSmoother;
    ParameterSmoother mixSmoother;
    ParameterSmoother hpfFreqSmoother;

    // Style crossfade
    ParameterSmoother styleCrossfade;
    int previousStyle = 0;
    int currentStyle = 0;

    double sampleRate = 44100.0;
    float currentGR = 0.0f;

    // DSP functions
    float processSidechainHPF(float input, float& hpfState, float freq);
    float computeDetector(float input, CompressorState& s, float attack, float release, Style style);
    float computeGainReduction(float envDB, float threshold, float ratio, float knee);
    float applySoftKnee(float inputDB, float threshold, float ratio, float knee);
};
