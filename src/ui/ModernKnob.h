#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class ModernKnob : public juce::Component
{
public:
    ModernKnob(const juce::String& labelText, juce::Colour accentColour = juce::Colour(0xff22d3ee));

    void setParameter(juce::RangedAudioParameter* param);
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::String label;
    juce::Colour colour;
    juce::Slider slider;
    juce::Label valueLabel;
    std::unique_ptr<juce::SliderParameterAttachment> attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModernKnob)
};
