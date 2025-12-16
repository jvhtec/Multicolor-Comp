#include "ColorModule.h"
#include <cmath>

ColorModule::ColorModule()
{
}

void ColorModule::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    // Create oversampling engines
    oversampling2x = std::make_unique<juce::dsp::Oversampling<float>>(
        static_cast<int>(spec.numChannels), 1,
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, false);

    oversampling4x = std::make_unique<juce::dsp::Oversampling<float>>(
        static_cast<int>(spec.numChannels), 2,
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, false);

    oversampling8x = std::make_unique<juce::dsp::Oversampling<float>>(
        static_cast<int>(spec.numChannels), 3,
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, false);

    oversampling2x->initProcessing(spec.maximumBlockSize);
    oversampling4x->initProcessing(spec.maximumBlockSize);
    oversampling8x->initProcessing(spec.maximumBlockSize);

    // Configure smoothers
    driveSmoother.setSampleRate(sampleRate);
    driveSmoother.setSmoothingTime(10.0f);

    toneSmoother.setSampleRate(sampleRate);
    toneSmoother.setSmoothingTime(10.0f);

    mixSmoother.setSampleRate(sampleRate);
    mixSmoother.setSmoothingTime(10.0f);

    outputSmoother.setSampleRate(sampleRate);
    outputSmoother.setSmoothingTime(10.0f);

    reset();
}

void ColorModule::reset()
{
    oversampling2x->reset();
    oversampling4x->reset();
    oversampling8x->reset();

    for (auto& state : dcBlockerState)
        state = 0.0f;
}

void ColorModule::process(juce::dsp::AudioBlock<float>& block, const Parameters& params)
{
    if (params.getBoolValue(ParamIDs::colorBypass))
        return;

    const int numChannels = static_cast<int>(block.getNumChannels());
    const int numSamples = static_cast<int>(block.getNumSamples());

    // Get parameters
    const int colorType = params.getIntValue(ParamIDs::colorType);
    const float drive = params.getValue(ParamIDs::colorDrive) * 0.01f;  // 0-1
    const float tone = params.getValue(ParamIDs::colorTone) * 0.01f;   // -0.5 to +0.5
    const float mix = params.getValue(ParamIDs::colorMix) * 0.01f;
    const float output = params.getValue(ParamIDs::colorOutput);
    const int osMode = params.getIntValue(ParamIDs::colorOS);

    // Update smoothers
    driveSmoother.setTarget(drive);
    toneSmoother.setTarget(tone);
    mixSmoother.setTarget(mix);
    outputSmoother.setTarget(juce::Decibels::decibelsToGain(output));

    // Store dry signal
    juce::AudioBuffer<float> dryBuffer(numChannels, numSamples);
    for (int ch = 0; ch < numChannels; ++ch)
        dryBuffer.copyFrom(ch, 0, block.getChannelPointer(ch), numSamples);

    // Choose oversampling
    juce::dsp::AudioBlock<float>* processBlock = &block;
    juce::dsp::AudioBlock<float> oversampledBlock;

    if (osMode == 2 || (osMode == 0 && drive > 0.5f))  // 2x
    {
        oversampledBlock = oversampling2x->processSamplesUp(block);
        processBlock = &oversampledBlock;
    }
    else if (osMode == 3 || (osMode == 0 && drive > 0.7f))  // 4x
    {
        oversampledBlock = oversampling4x->processSamplesUp(block);
        processBlock = &oversampledBlock;
    }
    else if (osMode == 4)  // 8x
    {
        oversampledBlock = oversampling8x->processSamplesUp(block);
        processBlock = &oversampledBlock;
    }

    // Process samples
    const int processNumSamples = static_cast<int>(processBlock->getNumSamples());

    for (int i = 0; i < processNumSamples; ++i)
    {
        const float drv = driveSmoother.getNext();
        const float tn = toneSmoother.getNext();

        for (int ch = 0; ch < numChannels; ++ch)
        {
            float sample = processBlock->getSample(ch, i);

            // Apply waveshaping based on color type
            switch (colorType)
            {
                case ColorType::Tape:
                    sample = processTape(sample, drv);
                    break;
                case ColorType::Tube:
                    sample = processTube(sample, drv);
                    break;
                case ColorType::Transformer:
                    sample = processTransformer(sample, drv);
                    break;
                case ColorType::Clip:
                    sample = processClip(sample, drv);
                    break;
            }

            // Tone control (placeholder - simple tilt)
            sample *= (1.0f + tn * 0.3f);

            processBlock->setSample(ch, i, sample);
        }
    }

    // Downsample if needed
    if (osMode >= 2 || (osMode == 0 && drive > 0.5f))
    {
        if (osMode == 2 || (osMode == 0 && drive <= 0.7f))
            oversampling2x->processSamplesDown(block);
        else if (osMode == 3)
            oversampling4x->processSamplesDown(block);
        else if (osMode == 4)
            oversampling8x->processSamplesDown(block);
    }

    // DC blocker and mix
    for (int i = 0; i < numSamples; ++i)
    {
        const float mx = mixSmoother.getNext();
        const float out = outputSmoother.getNext();

        for (int ch = 0; ch < numChannels; ++ch)
        {
            float wet = processDCBlocker(block.getSample(ch, i), dcBlockerState[ch]);
            wet *= out;

            float dry = dryBuffer.getSample(ch, i);
            block.setSample(ch, i, dry * (1.0f - mx) + wet * mx);
        }
    }
}

float ColorModule::processTape(float input, float drive)
{
    // Soft tape saturation
    const float x = input * (1.0f + drive * 3.0f);
    return std::tanh(x) * 0.9f;
}

float ColorModule::processTube(float input, float drive)
{
    // Even-harmonic emphasis
    const float x = input * (1.0f + drive * 4.0f);
    const float x2 = x * x;
    return x / (1.0f + x2) * 1.5f;
}

float ColorModule::processTransformer(float input, float drive)
{
    // Harder saturation with odd harmonics
    const float x = input * (1.0f + drive * 5.0f);
    const float abs_x = std::abs(x);

    if (abs_x < 0.33f)
        return 2.0f * x;
    else if (abs_x < 0.66f)
        return (x > 0.0f ? 1.0f : -1.0f) * (3.0f - std::pow(2.0f - 3.0f * abs_x, 2.0f)) / 3.0f;
    else
        return (x > 0.0f ? 1.0f : -1.0f);
}

float ColorModule::processClip(float input, float drive)
{
    // Hard clipping
    const float x = input * (1.0f + drive * 6.0f);
    return std::clamp(x, -1.0f, 1.0f);
}

float ColorModule::applyToneControl(float input, float tone, int channel)
{
    // Placeholder - real implementation would use filters
    return input;
}

float ColorModule::processDCBlocker(float input, float& state)
{
    // Simple one-pole DC blocker
    const float coeff = 0.995f;
    const float output = input - state;
    state = state * coeff + input * (1.0f - coeff);
    return output;
}
