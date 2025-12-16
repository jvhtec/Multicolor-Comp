#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "CompressorModule.h"
#include "ColorModule.h"
#include "SootheModule.h"
#include "../Parameters.h"

/**
 * Manages signal routing between the three main modules
 * Supports two routing options:
 *   A) Soothe → Compressor → Color (default)
 *   B) Compressor → Color → Soothe
 */
class RouterModule
{
public:
    RouterModule();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void process(juce::dsp::ProcessContextReplacing<float>& context, Parameters& params);

    float getGainReduction() const { return compressor.getGainReduction(); }
    int getLatencySamples() const;

private:
    CompressorModule compressor;
    ColorModule color;
    SootheModule soothe;

    // Global trim
    juce::dsp::Gain<float> inputGain;
    juce::dsp::Gain<float> outputGain;

    double sampleRate = 44100.0;

    // Routing
    void processRouteA(juce::dsp::AudioBlock<float>& block, Parameters& params);
    void processRouteB(juce::dsp::AudioBlock<float>& block, Parameters& params);
};
