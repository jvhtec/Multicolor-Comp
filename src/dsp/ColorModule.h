#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "Smoothing.h"
#include "../Parameters.h"

/**
 * Harmonic saturation with multiple color types
 * Includes oversampling to control aliasing
 */
class ColorModule
{
public:
    ColorModule();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void process(juce::dsp::AudioBlock<float>& block, const Parameters& params);

    enum ColorType
    {
        Tape = 0,
        Tube,
        Transformer,
        Clip
    };

private:
    // Oversampling
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampling2x;
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampling4x;
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampling8x;

    // Smoothers
    ParameterSmoother driveSmoother;
    ParameterSmoother toneSmoother;
    ParameterSmoother mixSmoother;
    ParameterSmoother outputSmoother;

    // DC blocker (simple one-pole HPF)
    float dcBlockerState[2] = {0.0f, 0.0f};

    double sampleRate = 44100.0;
    int currentOS = 0;

    // Waveshaping functions
    float processTape(float input, float drive);
    float processTube(float input, float drive);
    float processTransformer(float input, float drive);
    float processClip(float input, float drive);

    float applyToneControl(float input, float tone, int channel);
    float processDCBlocker(float input, float& state);
};
