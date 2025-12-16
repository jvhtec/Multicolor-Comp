#include "PluginEditor.h"

MultiColorCompEditor::MultiColorCompEditor(MultiColorCompProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(800, 600);
    startTimerHz(30);  // 30 fps for meter updates
}

MultiColorCompEditor::~MultiColorCompEditor()
{
    stopTimer();
}

void MultiColorCompEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a1a));

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawText("MULTI-COLOR COMP", getLocalBounds().removeFromTop(50),
               juce::Justification::centred, true);

    // Placeholder text
    g.setFont(14.0f);
    g.setColour(juce::Colours::grey);
    auto area = getLocalBounds().reduced(20);
    g.drawText("Week 1: DSP-first build in progress\nUI comes later",
               area, juce::Justification::centred, true);

    // Simple meter display
    area = getLocalBounds().withTrimmedTop(100).withHeight(30);
    g.setFont(12.0f);
    g.drawText(juce::String::formatted("In: L %.1f R %.1f | GR: %.1f | Out: L %.1f R %.1f",
                                       juce::Decibels::gainToDecibels(inputLevelL),
                                       juce::Decibels::gainToDecibels(inputLevelR),
                                       grLevel,
                                       juce::Decibels::gainToDecibels(outputLevelL),
                                       juce::Decibels::gainToDecibels(outputLevelR)),
               area, juce::Justification::centred, true);
}

void MultiColorCompEditor::resized()
{
}

void MultiColorCompEditor::timerCallback()
{
    inputLevelL = processor.getInputLevel(0);
    inputLevelR = processor.getInputLevel(1);
    outputLevelL = processor.getOutputLevel(0);
    outputLevelR = processor.getOutputLevel(1);
    grLevel = processor.getGainReduction();

    repaint();
}
