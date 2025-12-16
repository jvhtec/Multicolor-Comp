#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "Parameters.h"
#include "dsp/RouterModule.h"

class MultiColorCompProcessor : public juce::AudioProcessor
{
public:
    MultiColorCompProcessor();
    ~MultiColorCompProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Parameter access
    juce::AudioProcessorValueTreeState& getAPVTS() { return parameters.getAPVTS(); }
    Parameters& getParameters() { return parameters; }

    // Metering
    float getInputLevel(int channel) const { return inputLevel[channel].load(); }
    float getOutputLevel(int channel) const { return outputLevel[channel].load(); }
    float getGainReduction() const { return gainReduction.load(); }

private:
    Parameters parameters;
    RouterModule router;

    // Simple metering
    std::atomic<float> inputLevel[2] = {0.0f, 0.0f};
    std::atomic<float> outputLevel[2] = {0.0f, 0.0f};
    std::atomic<float> gainReduction{0.0f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiColorCompProcessor)
};
