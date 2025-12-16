#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class ModernLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ModernLookAndFeel();

    // Color scheme
    static const juce::Colour darkBg;           // #0f172a
    static const juce::Colour darkPanel;        // #1e293b
    static const juce::Colour darkCard;         // #334155
    static const juce::Colour darkBorder;       // #475569
    static const juce::Colour textPrimary;      // #e2e8f0
    static const juce::Colour textSecondary;    // #94a3b8
    static const juce::Colour cyan;             // #22d3ee
    static const juce::Colour amber;            // #fbbf24
    static const juce::Colour fuchsia;          // #e879f9
    static const juce::Colour emerald;          // #34d399

    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
                         float sliderPosProportional, float rotaryStartAngle,
                         float rotaryEndAngle, juce::Slider&) override;

    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
                             bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;
};
