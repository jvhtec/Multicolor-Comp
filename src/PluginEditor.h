#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "ui/ModernLookAndFeel.h"
#include "ui/ModernKnob.h"

class ModulePanel : public juce::Component
{
public:
    ModulePanel(const juce::String& title, juce::Colour accentColour);
    void paint(juce::Graphics& g) override;
    void setBypass(bool bypassed);
    bool isBypassed() const { return bypassed; }

protected:
    juce::String moduleTitle;
    juce::Colour colour;
    bool bypassed = false;
};

class CompressorPanel : public ModulePanel
{
public:
    CompressorPanel(MultiColorCompProcessor& p);
    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    MultiColorCompProcessor& processor;
    std::unique_ptr<ModernKnob> thresholdKnob, ratioKnob, attackKnob, releaseKnob, kneeKnob, mixKnob;
    juce::TextButton vcaButton, fetButton, optoButton, varimuButton;
    std::unique_ptr<juce::ButtonParameterAttachment> styleAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorPanel)
};

class ColorPanel : public ModulePanel
{
public:
    ColorPanel(MultiColorCompProcessor& p);
    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    MultiColorCompProcessor& processor;
    std::unique_ptr<ModernKnob> driveKnob, toneKnob, mixKnob, outputKnob;
    juce::TextButton tapeButton, tubeButton, transButton, clipButton;
    std::unique_ptr<juce::ButtonParameterAttachment> typeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColorPanel)
};

class SoothePanel : public ModulePanel
{
public:
    SoothePanel(MultiColorCompProcessor& p);
    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    MultiColorCompProcessor& processor;
    std::unique_ptr<ModernKnob> amountKnob, sensitivityKnob, sharpnessKnob;
    std::unique_ptr<ModernKnob> speedKnob, focusLowKnob, focusHighKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoothePanel)
};

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
    ModernLookAndFeel modernLNF;

    CompressorPanel compressorPanel;
    ColorPanel colorPanel;
    SoothePanel soothePanel;

    ModernKnob intensityKnob;
    juce::TextButton routingButton;

    // Metering
    float inputLevelL = 0.0f, inputLevelR = 0.0f;
    float outputLevelL = 0.0f, outputLevelR = 0.0f;
    float grLevel = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiColorCompEditor)
};
