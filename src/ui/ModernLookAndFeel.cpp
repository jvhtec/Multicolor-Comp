#include "ModernLookAndFeel.h"

const juce::Colour ModernLookAndFeel::darkBg        = juce::Colour(0xff0f172a);
const juce::Colour ModernLookAndFeel::darkPanel     = juce::Colour(0xff1e293b);
const juce::Colour ModernLookAndFeel::darkCard      = juce::Colour(0xff334155);
const juce::Colour ModernLookAndFeel::darkBorder    = juce::Colour(0xff475569);
const juce::Colour ModernLookAndFeel::textPrimary   = juce::Colour(0xffe2e8f0);
const juce::Colour ModernLookAndFeel::textSecondary = juce::Colour(0xff94a3b8);
const juce::Colour ModernLookAndFeel::cyan          = juce::Colour(0xff22d3ee);
const juce::Colour ModernLookAndFeel::amber         = juce::Colour(0xfffbbf24);
const juce::Colour ModernLookAndFeel::fuchsia       = juce::Colour(0xffe879f9);
const juce::Colour ModernLookAndFeel::emerald       = juce::Colour(0xff34d399);

ModernLookAndFeel::ModernLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, darkBg);
    setColour(juce::Slider::rotarySliderFillColourId, cyan);
    setColour(juce::Slider::thumbColourId, juce::Colours::white);
    setColour(juce::TextButton::buttonColourId, darkCard);
}

void ModernLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                        float sliderPos, float rotaryStartAngle,
                                        float rotaryEndAngle, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = juce::jmin(8.0f, radius * 0.3f);
    auto arcRadius = radius - lineW * 0.5f;

    // Background arc
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(),
                               bounds.getCentreY(),
                               arcRadius, arcRadius,
                               0.0f,
                               rotaryStartAngle,
                               rotaryEndAngle,
                               true);

    g.setColour(darkBorder);
    g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Value arc
    if (sliderPos > 0.0f)
    {
        juce::Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(),
                              bounds.getCentreY(),
                              arcRadius, arcRadius,
                              0.0f,
                              rotaryStartAngle,
                              toAngle,
                              true);

        auto colour = slider.findColour(juce::Slider::rotarySliderFillColourId);
        g.setColour(colour);
        g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Pointer
    juce::Path pointer;
    auto pointerLength = radius * 0.3f;
    auto pointerThickness = 3.0f;
    pointer.addRectangle(-pointerThickness * 0.5f, -radius + lineW, pointerThickness, pointerLength);
    pointer.applyTransform(juce::AffineTransform::rotation(toAngle).translated(bounds.getCentreX(), bounds.getCentreY()));

    g.setColour(juce::Colours::white);
    g.fillPath(pointer);
}

void ModernLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                            const juce::Colour& backgroundColour,
                                            bool shouldDrawButtonAsHighlighted,
                                            bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);
    auto cornerSize = 6.0f;

    auto baseColour = backgroundColour;

    if (button.getToggleState())
    {
        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, cornerSize);
    }
    else
    {
        g.setColour(darkCard);
        g.fillRoundedRectangle(bounds, cornerSize);

        if (shouldDrawButtonAsHighlighted)
        {
            g.setColour(darkBorder);
            g.fillRoundedRectangle(bounds, cornerSize);
        }
    }

    // Border
    g.setColour(button.getToggleState() ? backgroundColour.darker(0.3f) : darkBorder);
    g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
}

juce::Font ModernLookAndFeel::getTextButtonFont(juce::TextButton&, int buttonHeight)
{
    return juce::Font(10.0f, juce::Font::bold);
}
