#include "SootheModule.h"
#include <cmath>
#include <algorithm>

SootheModule::SootheModule()
{
}

void SootheModule::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    // Set FFT parameters based on quality (will be updated in process())
    fftSize = 1024;  // Default to Normal
    hopSize = 256;
    currentQuality = Quality::Normal;

    // Create FFT
    fft = std::make_unique<juce::dsp::FFT>(static_cast<int>(std::log2(fftSize)));

    // Prepare channel states
    for (auto& state : channelState)
    {
        state.inputFIFO.resize(fftSize, 0.0f);
        state.outputFIFO.resize(fftSize, 0.0f);
        state.fftData.resize(fftSize * 2, 0.0f);
        state.ifftData.resize(fftSize * 2, 0.0f);
        state.window.resize(fftSize);
        state.magnitudes.resize(fftSize / 2 + 1, 0.0f);
        state.baseline.resize(fftSize / 2 + 1, 0.0f);
        state.attenuation.resize(fftSize / 2 + 1, 1.0f);
        state.resonanceScore.resize(fftSize / 2 + 1, 0.0f);
        state.overlapBuffer.resize(fftSize, 0.0f);

        createWindow(state.window, fftSize);
    }

    latencySamples = fftSize;

    reset();
}

void SootheModule::reset()
{
    for (auto& state : channelState)
        state.reset();
}

void SootheModule::process(juce::dsp::AudioBlock<float>& block, const Parameters& params)
{
    if (params.getBoolValue(ParamIDs::sootheBypass))
        return;

    // Check if quality mode has changed and reinitialize if needed
    int quality = params.getIntValue(ParamIDs::sootheQuality);
    Quality newQuality = (quality == 0) ? Quality::Eco : (quality == 1) ? Quality::Normal : Quality::High;

    if (newQuality != currentQuality)
    {
        currentQuality = newQuality;

        // Update FFT size based on quality
        if (currentQuality == Quality::Eco)
            fftSize = 512;
        else if (currentQuality == Quality::Normal)
            fftSize = 1024;
        else
            fftSize = 2048;

        hopSize = fftSize / 4;

        // Recreate FFT and resize buffers
        fft = std::make_unique<juce::dsp::FFT>(static_cast<int>(std::log2(fftSize)));

        for (auto& state : channelState)
        {
            state.inputFIFO.resize(fftSize, 0.0f);
            state.outputFIFO.resize(fftSize, 0.0f);
            state.fftData.resize(fftSize * 2, 0.0f);
            state.ifftData.resize(fftSize * 2, 0.0f);
            state.window.resize(fftSize);
            state.magnitudes.resize(fftSize / 2 + 1, 0.0f);
            state.baseline.resize(fftSize / 2 + 1, 0.0f);
            state.attenuation.resize(fftSize / 2 + 1, 1.0f);
            state.resonanceScore.resize(fftSize / 2 + 1, 0.0f);
            state.overlapBuffer.resize(fftSize, 0.0f);

            createWindow(state.window, fftSize);
            state.fifoIndex = 0;
            state.hopCounter = 0;
        }

        latencySamples = fftSize;
    }

    const int numChannels = std::min(2, static_cast<int>(block.getNumChannels()));
    const int numSamples = static_cast<int>(block.getNumSamples());

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto& state = channelState[ch];
        auto* channelData = block.getChannelPointer(ch);

        for (int i = 0; i < numSamples; ++i)
        {
            // Store input in FIFO
            state.inputFIFO[state.fifoIndex] = channelData[i];

            // Output from FIFO (delay compensation)
            channelData[i] = state.outputFIFO[state.fifoIndex];

            state.fifoIndex++;

            // Process when hop is reached
            if (state.fifoIndex >= hopSize)
            {
                state.fifoIndex = 0;
                state.hopCounter++;

                if (state.hopCounter >= (fftSize / hopSize))
                {
                    state.hopCounter = 0;
                    processFFTFrame(state, params);
                }
            }
        }
    }
}

void SootheModule::processFFTFrame(ChannelState& state, const Parameters& params)
{
    // Copy input with window
    for (int i = 0; i < fftSize; ++i)
    {
        state.fftData[i] = state.inputFIFO[i] * state.window[i];
        state.fftData[fftSize + i] = 0.0f;  // Zero imaginary part
    }

    // Forward FFT
    fft->performRealOnlyForwardTransform(state.fftData.data(), true);

    // Compute magnitudes
    for (int k = 0; k <= fftSize / 2; ++k)
    {
        const float real = state.fftData[k * 2];
        const float imag = state.fftData[k * 2 + 1];
        state.magnitudes[k] = std::sqrt(real * real + imag * imag);
    }

    // Get parameters
    const float amount = params.getValue(ParamIDs::sootheAmount) * 0.01f;
    const float sensitivity = params.getValue(ParamIDs::sootheSensitivity) * 0.01f;
    const float sharpness = params.getValue(ParamIDs::sootheSharpness) * 0.01f;
    const float speed = params.getValue(ParamIDs::sootheSpeed) * 0.01f;
    const float focusLow = params.getValue(ParamIDs::sootheFocusLow);
    const float focusHigh = params.getValue(ParamIDs::sootheFocusHigh);
    const float mix = params.getValue(ParamIDs::sootheMix) * 0.01f;
    const bool deltaMode = params.getBoolValue(ParamIDs::sootheDelta);

    // Compute baseline
    const float smoothingWidth = 5.0f + sharpness * 20.0f;
    computeBaseline(state, smoothingWidth);

    // Compute resonance scores
    computeResonanceScore(state, sensitivity, focusLow, focusHigh);

    // Update attenuation
    updateAttenuation(state, amount, speed, sharpness);

    // Apply attenuation to complex spectrum
    for (int k = 0; k <= fftSize / 2; ++k)
    {
        state.fftData[k * 2] *= state.attenuation[k];
        state.fftData[k * 2 + 1] *= state.attenuation[k];
    }

    // Inverse FFT
    std::copy(state.fftData.begin(), state.fftData.end(), state.ifftData.begin());
    fft->performRealOnlyInverseTransform(state.ifftData.data());

    // Overlap-add with window
    for (int i = 0; i < fftSize; ++i)
    {
        state.overlapBuffer[i] += state.ifftData[i] * state.window[i] / static_cast<float>(fftSize / hopSize);
    }

    // Copy to output FIFO
    for (int i = 0; i < hopSize; ++i)
    {
        float processed = state.overlapBuffer[i];

        // Mix
        float dry = state.inputFIFO[i];
        float output = dry * (1.0f - mix) + processed * mix;

        // Delta mode
        if (deltaMode)
            output = dry - processed;

        state.outputFIFO[i] = output;
    }

    // Shift overlap buffer
    std::copy(state.overlapBuffer.begin() + hopSize, state.overlapBuffer.end(), state.overlapBuffer.begin());
    std::fill(state.overlapBuffer.end() - hopSize, state.overlapBuffer.end(), 0.0f);

    // Shift input FIFO
    std::copy(state.inputFIFO.begin() + hopSize, state.inputFIFO.end(), state.inputFIFO.begin());
    std::fill(state.inputFIFO.end() - hopSize, state.inputFIFO.end(), 0.0f);
}

void SootheModule::computeBaseline(ChannelState& state, float smoothingWidth)
{
    // Simple moving average baseline
    const int width = static_cast<int>(smoothingWidth);

    for (int k = 0; k <= fftSize / 2; ++k)
    {
        float sum = 0.0f;
        int count = 0;

        for (int n = std::max(0, k - width); n <= std::min(fftSize / 2, k + width); ++n)
        {
            sum += state.magnitudes[n];
            count++;
        }

        state.baseline[k] = (count > 0) ? sum / static_cast<float>(count) : state.magnitudes[k];
    }
}

void SootheModule::computeResonanceScore(ChannelState& state, float sensitivity,
                                        float focusLow, float focusHigh)
{
    const int binLow = freqToFFTBin(focusLow);
    const int binHigh = freqToFFTBin(focusHigh);

    for (int k = 0; k <= fftSize / 2; ++k)
    {
        // Outside focus range
        if (k < binLow || k > binHigh)
        {
            state.resonanceScore[k] = 0.0f;
            continue;
        }

        // Ratio of magnitude to baseline
        const float ratio = state.magnitudes[k] / (state.baseline[k] + 1e-10f);

        // Resonance score
        float score = std::max(0.0f, ratio - 1.0f);
        score = std::pow(score, 1.5f);  // Selectivity

        state.resonanceScore[k] = score * sensitivity;
    }
}

void SootheModule::updateAttenuation(ChannelState& state, float amount,
                                    float speed, float sharpness)
{
    const float maxAttnDB = -12.0f * amount;
    const float attackCoeff = 0.1f + speed * 0.4f;
    const float releaseCoeff = 0.01f + speed * 0.09f;

    for (int k = 0; k <= fftSize / 2; ++k)
    {
        // Target attenuation in dB
        const float targetAttnDB = -state.resonanceScore[k] * 12.0f * amount;
        const float clampedAttnDB = std::max(targetAttnDB, maxAttnDB);
        const float targetAttn = juce::Decibels::decibelsToGain(clampedAttnDB);

        // Smooth attenuation over time
        const float coeff = (targetAttn < state.attenuation[k]) ? attackCoeff : releaseCoeff;
        state.attenuation[k] += (targetAttn - state.attenuation[k]) * coeff;

        // Clamp
        state.attenuation[k] = std::clamp(state.attenuation[k], 0.1f, 1.0f);
    }

    // Frequency smoothing based on sharpness
    const int smoothWidth = static_cast<int>(1.0f + (1.0f - sharpness) * 5.0f);
    std::vector<float> smoothed = state.attenuation;
    smoothSpectrum(state.attenuation, smoothed, smoothWidth);
    state.attenuation = smoothed;
}

void SootheModule::applyAttenuation(ChannelState& state)
{
    // Already applied in processFFTFrame
}

void SootheModule::createWindow(std::vector<float>& window, int size)
{
    // Hann window
    for (int i = 0; i < size; ++i)
    {
        window[i] = 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * static_cast<float>(i) / static_cast<float>(size - 1)));
    }
}

void SootheModule::smoothSpectrum(const std::vector<float>& input,
                                 std::vector<float>& output, int width)
{
    const int size = static_cast<int>(input.size());

    for (int k = 0; k < size; ++k)
    {
        float sum = 0.0f;
        int count = 0;

        for (int n = std::max(0, k - width); n <= std::min(size - 1, k + width); ++n)
        {
            sum += input[n];
            count++;
        }

        output[k] = (count > 0) ? sum / static_cast<float>(count) : input[k];
    }
}

int SootheModule::freqToFFTBin(float freq) const
{
    return static_cast<int>((freq / static_cast<float>(sampleRate)) * static_cast<float>(fftSize));
}
