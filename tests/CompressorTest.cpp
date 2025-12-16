#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "../src/dsp/CompressorModule.h"
#include "../src/dsp/ColorModule.h"
#include "../src/dsp/SootheModule.h"
#include "../src/Parameters.h"
#include <iostream>
#include <cassert>

// Simple test parameter provider
class TestParameters : public Parameters
{
public:
    TestParameters() : Parameters(dummyProcessor) {}

private:
    // Dummy processor for testing
    class DummyProcessor : public juce::AudioProcessor
    {
    public:
        DummyProcessor() : juce::AudioProcessor(BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo())
            .withOutput("Output", juce::AudioChannelSet::stereo())) {}

        const juce::String getName() const override { return "Dummy"; }
        void prepareToPlay(double, int) override {}
        void releaseResources() override {}
        void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override {}
        juce::AudioProcessorEditor* createEditor() override { return nullptr; }
        bool hasEditor() const override { return false; }
        int getNumPrograms() override { return 1; }
        int getCurrentProgram() override { return 0; }
        void setCurrentProgram(int) override {}
        const juce::String getProgramName(int) override { return {}; }
        void changeProgramName(int, const juce::String&) override {}
        void getStateInformation(juce::MemoryBlock&) override {}
        void setStateInformation(const void*, int) override {}
        double getTailLengthSeconds() const override { return 0.0; }
        bool acceptsMidi() const override { return false; }
        bool producesMidi() const override { return false; }
    };

    DummyProcessor dummyProcessor;
};

void testCompressor()
{
    std::cout << "Testing Compressor Module..." << std::endl;

    CompressorModule comp;
    TestParameters params;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = 44100.0;
    spec.maximumBlockSize = 512;
    spec.numChannels = 2;

    comp.prepare(spec);

    // Create test buffer with 1 kHz sine wave at -6 dB
    juce::AudioBuffer<float> buffer(2, 512);
    for (int ch = 0; ch < 2; ++ch)
    {
        for (int i = 0; i < 512; ++i)
        {
            float t = static_cast<float>(i) / 44100.0f;
            buffer.setSample(ch, i, 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * 1000.0f * t));
        }
    }

    // Process
    juce::dsp::AudioBlock<float> block(buffer);
    comp.process(block, params);

    // Check output is not silent and not clipping
    float maxLevel = 0.0f;
    for (int ch = 0; ch < 2; ++ch)
    {
        for (int i = 0; i < 512; ++i)
        {
            maxLevel = std::max(maxLevel, std::abs(buffer.getSample(ch, i)));
        }
    }

    assert(maxLevel > 0.0f && "Output is silent");
    assert(maxLevel <= 1.0f && "Output is clipping");

    std::cout << "  Max level: " << juce::Decibels::gainToDecibels(maxLevel) << " dB" << std::endl;
    std::cout << "  Gain reduction: " << comp.getGainReduction() << " dB" << std::endl;
    std::cout << "  ✓ Compressor test passed" << std::endl;
}

void testColor()
{
    std::cout << "\nTesting Color Module..." << std::endl;

    ColorModule color;
    TestParameters params;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = 44100.0;
    spec.maximumBlockSize = 512;
    spec.numChannels = 2;

    color.prepare(spec);

    // Create test buffer
    juce::AudioBuffer<float> buffer(2, 512);
    for (int ch = 0; ch < 2; ++ch)
    {
        for (int i = 0; i < 512; ++i)
        {
            float t = static_cast<float>(i) / 44100.0f;
            buffer.setSample(ch, i, 0.3f * std::sin(2.0f * juce::MathConstants<float>::pi * 440.0f * t));
        }
    }

    // Process
    juce::dsp::AudioBlock<float> block(buffer);
    color.process(block, params);

    // Check output
    float maxLevel = 0.0f;
    for (int ch = 0; ch < 2; ++ch)
    {
        for (int i = 0; i < 512; ++i)
        {
            maxLevel = std::max(maxLevel, std::abs(buffer.getSample(ch, i)));
        }
    }

    assert(maxLevel > 0.0f && "Output is silent");
    std::cout << "  Max level: " << juce::Decibels::gainToDecibels(maxLevel) << " dB" << std::endl;
    std::cout << "  ✓ Color test passed" << std::endl;
}

void testBypass()
{
    std::cout << "\nTesting Bypass (null test)..." << std::endl;

    CompressorModule comp;
    TestParameters params;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = 44100.0;
    spec.maximumBlockSize = 512;
    spec.numChannels = 2;

    comp.prepare(spec);

    // Create test buffer
    juce::AudioBuffer<float> buffer(2, 512);
    juce::AudioBuffer<float> original(2, 512);

    for (int ch = 0; ch < 2; ++ch)
    {
        for (int i = 0; i < 512; ++i)
        {
            float t = static_cast<float>(i) / 44100.0f;
            float sample = 0.5f * std::sin(2.0f * juce::MathConstants<float>::pi * 1000.0f * t);
            buffer.setSample(ch, i, sample);
            original.setSample(ch, i, sample);
        }
    }

    // Enable bypass
    params.getAPVTS().getParameter(ParamIDs::compBypass)->setValueNotifyingHost(1.0f);

    // Process
    juce::dsp::AudioBlock<float> block(buffer);
    comp.process(block, params);

    // Check that output matches input
    float maxDiff = 0.0f;
    for (int ch = 0; ch < 2; ++ch)
    {
        for (int i = 0; i < 512; ++i)
        {
            float diff = std::abs(buffer.getSample(ch, i) - original.getSample(ch, i));
            maxDiff = std::max(maxDiff, diff);
        }
    }

    assert(maxDiff < 0.0001f && "Bypass is not working correctly");
    std::cout << "  Max difference: " << maxDiff << std::endl;
    std::cout << "  ✓ Bypass test passed" << std::endl;
}

int main(int argc, char* argv[])
{
    std::cout << "=== Multi-Color Comp DSP Tests ===" << std::endl;
    std::cout << std::endl;

    try
    {
        testCompressor();
        testColor();
        testBypass();

        std::cout << "\n=== All tests passed! ===" << std::endl;
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n!!! Test failed: " << e.what() << std::endl;
        return 1;
    }
}
