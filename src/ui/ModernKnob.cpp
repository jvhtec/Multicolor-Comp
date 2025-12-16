#include "ModernKnob.h"
#include "ModernLookAndFeel.h"

ModernKnob::ModernKnob(const juce::String& labelText, juce::Colour accentColour)
    : label(labelText), colour(accentColour)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider.setColour(juce::Slider::rotarySliderFillColourId, colour);
    addAndMakeVisible(slider);

    valueLabel.setFont(juce::Font(9.0f, juce::Font::plain));
    valueLabel.setJustificationType(juce::Justification::centred);
    valueLabel.setColour(juce::Label::textColourId, colour);
    addAndMakeVisible(valueLabel);

    slider.onValueChange = [this]()
    {
        valueLabel.setText(juce::String((int)slider.getValue()), juce::dontSendNotification);
    };
}

void ModernKnob::setParameter(juce::RangedAudioParameter* param)
{
    if (param)
    {
        attachment = std::make_unique<juce::SliderParameterAttachment>(*param, slider);
        valueLabel.setText(juce::String((int)slider.getValue()), juce::dontSendNotification);
    }
}

void ModernKnob::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    // Label
    g.setColour(ModernLookAndFeel::textSecondary);
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText(label, bounds.removeFromBottom(18), juce::Justification::centred);
}

void ModernKnob::resized()
{
    auto bounds = getLocalBounds();

    valueLabel.setBounds(bounds.removeFromBottom(12));
    bounds.removeFromBottom(6); // spacing

    auto knobSize = juce::jmin(bounds.getWidth(), bounds.getHeight() - 12);
    slider.setBounds(bounds.withSizeKeepingCentre(knobSize, knobSize));
}
