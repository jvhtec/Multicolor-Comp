#include "CompressorModule.h"
#include <cmath>

CompressorModule::CompressorModule()
{
    previousStyle = Style::VCA;
    currentStyle = Style::VCA;
}

void CompressorModule::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    // Configure smoothers
    attackSmoother.setSampleRate(sampleRate);
    attackSmoother.setSmoothingTime(10.0f);

    releaseSmoother.setSampleRate(sampleRate);
    releaseSmoother.setSmoothingTime(10.0f);

    thresholdSmoother.setSampleRate(sampleRate);
    thresholdSmoother.setSmoothingTime(5.0f);

    ratioSmoother.setSampleRate(sampleRate);
    ratioSmoother.setSmoothingTime(5.0f);

    kneeSmoother.setSampleRate(sampleRate);
    kneeSmoother.setSmoothingTime(5.0f);

    makeupSmoother.setSampleRate(sampleRate);
    makeupSmoother.setSmoothingTime(10.0f);

    mixSmoother.setSampleRate(sampleRate);
    mixSmoother.setSmoothingTime(10.0f);

    hpfFreqSmoother.setSampleRate(sampleRate);
    hpfFreqSmoother.setSmoothingTime(20.0f);

    styleCrossfade.setSampleRate(sampleRate);
    styleCrossfade.setSmoothingTime(50.0f);  // 50ms style crossfade

    reset();
}

void CompressorModule::reset()
{
    for (auto& s : state)
        s.reset();

    currentGR = 0.0f;
}

void CompressorModule::process(juce::dsp::AudioBlock<float>& block, const Parameters& params)
{
    if (params.getBoolValue(ParamIDs::compBypass))
        return;

    const int numChannels = static_cast<int>(block.getNumChannels());
    const int numSamples = static_cast<int>(block.getNumSamples());

    // Update style
    int newStyle = params.getIntValue(ParamIDs::compStyle);
    if (newStyle != currentStyle)
    {
        previousStyle = currentStyle;
        currentStyle = newStyle;
        styleCrossfade.setTarget(0.0f);
        styleCrossfade.reset(1.0f);
    }

    // Get parameters
    const float threshold = params.getModulatedThreshold();  // Modulated by intensity macro
    const float ratio = params.getValue(ParamIDs::compRatio);
    const float attack = params.getValue(ParamIDs::compAttack);
    const float release = params.getValue(ParamIDs::compRelease);
    const float knee = params.getValue(ParamIDs::compKnee);
    const float makeup = params.getModulatedMakeup();  // Modulated by intensity macro
    const float mix = params.getValue(ParamIDs::compMix) * 0.01f;  // 0-1
    const float hpfFreq = params.getValue(ParamIDs::compSCHPF);
    const int stereoLink = params.getIntValue(ParamIDs::compStereoLink);

    // Update smoothers
    thresholdSmoother.setTarget(threshold);
    ratioSmoother.setTarget(ratio);
    attackSmoother.setTarget(attack);
    releaseSmoother.setTarget(release);
    kneeSmoother.setTarget(knee);
    makeupSmoother.setTarget(makeup);
    mixSmoother.setTarget(mix);
    hpfFreqSmoother.setTarget(hpfFreq);

    for (int i = 0; i < numSamples; ++i)
    {
        // Get smoothed parameters
        const float threshDB = thresholdSmoother.getNext();
        const float rat = ratioSmoother.getNext();
        const float att = attackSmoother.getNext();
        const float rel = releaseSmoother.getNext();
        const float kn = kneeSmoother.getNext();
        const float mk = makeupSmoother.getNext();
        const float mx = mixSmoother.getNext();
        const float hpf = hpfFreqSmoother.getNext();

        // Compute detector level for each channel
        float detectorLevels[2] = {-100.0f, -100.0f};

        for (int ch = 0; ch < numChannels; ++ch)
        {
            float sample = block.getSample(ch, i);

            // Sidechain HPF
            sample = processSidechainHPF(sample, state[ch].hpfState, hpf);

            // Detector
            detectorLevels[ch] = computeDetector(sample, state[ch], att, rel,
                                                 static_cast<Style>(currentStyle));
        }

        // Stereo linking
        float linkedDetectorDB = detectorLevels[0];
        if (numChannels == 2)
        {
            if (stereoLink == 1)  // Average
                linkedDetectorDB = (detectorLevels[0] + detectorLevels[1]) * 0.5f;
            else if (stereoLink == 2)  // Max
                linkedDetectorDB = std::max(detectorLevels[0], detectorLevels[1]);
        }

        // Compute gain reduction
        const float grDB = computeGainReduction(linkedDetectorDB, threshDB, rat, kn);
        const float targetGain = juce::Decibels::decibelsToGain(grDB + mk);

        // Smooth gain (prevents zipper)
        const float gainSmoothCoeff = 0.01f;  // ~1-2ms at 44.1k

        // Apply to all channels
        for (int ch = 0; ch < numChannels; ++ch)
        {
            state[ch].gainLinear += (targetGain - state[ch].gainLinear) * gainSmoothCoeff;

            // Apply compression
            float dry = block.getSample(ch, i);
            float wet = dry * state[ch].gainLinear;

            // Parallel mix
            block.setSample(ch, i, dry * (1.0f - mx) + wet * mx);
        }

        // Store GR for metering (use first channel)
        currentGR = grDB;
    }
}

float CompressorModule::processSidechainHPF(float input, float& hpfState, float freq)
{
    // Simple one-pole HPF
    const float normalizedFreq = freq / static_cast<float>(sampleRate);
    const float coeff = std::exp(-juce::MathConstants<float>::twoPi * normalizedFreq);

    hpfState = coeff * (hpfState + input - input);
    return input - hpfState;
}

float CompressorModule::computeDetector(float input, CompressorState& s,
                                       float attack, float release, Style style)
{
    // Compute power
    const float power = input * input;

    // RMS envelope
    const float rmsCoeff = 1.0f - std::exp(-1.0f / (5.0f * 0.001f * static_cast<float>(sampleRate)));
    s.envelopeRMS += (power - s.envelopeRMS) * rmsCoeff;
    const float rms = std::sqrt(s.envelopeRMS + 1e-10f);

    // Peak envelope
    const float peakAttackCoeff = 1.0f - std::exp(-1.0f / (0.1f * 0.001f * static_cast<float>(sampleRate)));
    const float peakReleaseCoeff = 1.0f - std::exp(-1.0f / (10.0f * 0.001f * static_cast<float>(sampleRate)));
    const float absSample = std::abs(input);

    if (absSample > s.envelopePeak)
        s.envelopePeak += (absSample - s.envelopePeak) * peakAttackCoeff;
    else
        s.envelopePeak += (absSample - s.envelopePeak) * peakReleaseCoeff;

    // Blend peak/RMS based on style
    float blend = 0.3f;  // VCA default

    switch (style)
    {
        case FET:
            blend = 0.8f;  // More peak detection
            break;
        case Opto:
            blend = 0.1f;  // Mostly RMS
            break;
        case VariMu:
            blend = 0.2f;  // Mostly RMS
            break;
        default:
            break;
    }

    const float detector = blend * s.envelopePeak + (1.0f - blend) * rms;
    const float detectorDB = juce::Decibels::gainToDecibels(detector + 1e-10f);

    // Attack/Release on detector level
    const float attackCoeff = 1.0f - std::exp(-1.0f / (attack * 0.001f * static_cast<float>(sampleRate)));
    const float releaseCoeff = 1.0f - std::exp(-1.0f / (release * 0.001f * static_cast<float>(sampleRate)));

    const float coeff = (detectorDB > s.envelopeDB) ? attackCoeff : releaseCoeff;
    s.envelopeDB += (detectorDB - s.envelopeDB) * coeff;

    return s.envelopeDB;
}

float CompressorModule::computeGainReduction(float envDB, float threshold, float ratio, float knee)
{
    if (knee < 0.1f)
    {
        // Hard knee
        if (envDB <= threshold)
            return 0.0f;

        return (threshold + (envDB - threshold) / ratio) - envDB;
    }
    else
    {
        // Soft knee
        return applySoftKnee(envDB, threshold, ratio, knee);
    }
}

float CompressorModule::applySoftKnee(float inputDB, float threshold, float ratio, float knee)
{
    const float lower = threshold - knee * 0.5f;
    const float upper = threshold + knee * 0.5f;

    if (inputDB < lower)
    {
        return 0.0f;
    }
    else if (inputDB > upper)
    {
        return (threshold + (inputDB - threshold) / ratio) - inputDB;
    }
    else
    {
        // Within knee region - smooth quadratic transition
        const float x = inputDB - lower;
        const float compressedDB = inputDB + (1.0f / ratio - 1.0f) * (x * x) / (2.0f * knee);
        return compressedDB - inputDB;
    }
}
