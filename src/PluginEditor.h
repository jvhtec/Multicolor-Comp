#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class MultiColorCompEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit MultiColorCompEditor(MultiColorCompProcessor&);
    ~MultiColorCompEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    MultiColorCompProcessor& processor;

    // Metering
    float inputLevelL = 0.0f;
    float inputLevelR = 0.0f;
    float outputLevelL = 0.0f;
    float outputLevelR = 0.0f;
    float grLevel = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiColorCompEditor)
};
