#include "PluginEditor.h"

// ============================================================================
// ModulePanel Base Class
// ============================================================================

ModulePanel::ModulePanel(const juce::String& title, juce::Colour accentColour)
    : moduleTitle(title), colour(accentColour)
{
}

void ModulePanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    // Panel background
    g.setColour(ModernLookAndFeel::darkCard.withAlpha(0.5f));
    g.fillRoundedRectangle(bounds.toFloat(), 12.0f);

    // Border glow when active
    if (!bypassed)
    {
        g.setColour(colour.withAlpha(0.3f));
        g.drawRoundedRectangle(bounds.toFloat().reduced(0.5f), 12.0f, 1.5f);
    }

    // Bypass overlay
    if (bypassed)
    {
        g.setColour(ModernLookAndFeel::darkBg.withAlpha(0.8f));
        g.fillRoundedRectangle(bounds.toFloat(), 12.0f);
    }

    // Header
    auto headerBounds = bounds.removeFromTop(40);
    g.setColour(bypassed ? ModernLookAndFeel::textSecondary.withAlpha(0.5f) : colour);
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.drawText(moduleTitle.toUpperCase(), headerBounds.reduced(12, 0),
               juce::Justification::centredLeft);
}

void ModulePanel::setBypass(bool shouldBypass)
{
    bypassed = shouldBypass;
    repaint();
}

// ============================================================================
// CompressorPanel
// ============================================================================

CompressorPanel::CompressorPanel(MultiColorCompProcessor& p)
    : ModulePanel("COMPRESSOR", ModernLookAndFeel::cyan), processor(p)
{
    // Style buttons
    vcaButton.setButtonText("VCA");
    vcaButton.setClickingTogglesState(true);
    vcaButton.setRadioGroupId(1);
    vcaButton.setColour(juce::TextButton::buttonOnColourId, ModernLookAndFeel::cyan);
    addAndMakeVisible(vcaButton);

    fetButton.setButtonText("FET");
    fetButton.setClickingTogglesState(true);
    fetButton.setRadioGroupId(1);
    fetButton.setColour(juce::TextButton::buttonOnColourId, ModernLookAndFeel::cyan);
    addAndMakeVisible(fetButton);

    optoButton.setButtonText("OPTO");
    optoButton.setClickingTogglesState(true);
    optoButton.setRadioGroupId(1);
    optoButton.setColour(juce::TextButton::buttonOnColourId, ModernLookAndFeel::cyan);
    addAndMakeVisible(optoButton);

    varimuButton.setButtonText("VARI-MU");
    varimuButton.setClickingTogglesState(true);
    varimuButton.setRadioGroupId(1);
    varimuButton.setColour(juce::TextButton::buttonOnColourId, ModernLookAndFeel::cyan);
    addAndMakeVisible(varimuButton);

    // Create knobs
    thresholdKnob = std::make_unique<ModernKnob>("THRESH", ModernLookAndFeel::cyan);
    thresholdKnob->setParameter(processor.getAPVTS().getParameter(ParamIDs::compThreshold));
    addAndMakeVisible(*thresholdKnob);

    ratioKnob = std::make_unique<ModernKnob>("RATIO", ModernLookAndFeel::cyan);
    ratioKnob->setParameter(processor.getAPVTS().getParameter(ParamIDs::compRatio));
    addAndMakeVisible(*ratioKnob);

    attackKnob = std::make_unique<ModernKnob>("ATTACK", ModernLookAndFeel::cyan);
    attackKnob->setParameter(processor.getAPVTS().getParameter(ParamIDs::compAttack));
    addAndMakeVisible(*attackKnob);

    releaseKnob = std::make_unique<ModernKnob>("RELEASE", ModernLookAndFeel::cyan);
    releaseKnob->setParameter(processor.getAPVTS().getParameter(ParamIDs::compRelease));
    addAndMakeVisible(*releaseKnob);

    kneeKnob = std::make_unique<ModernKnob>("KNEE", ModernLookAndFeel::cyan);
    kneeKnob->setParameter(processor.getAPVTS().getParameter(ParamIDs::compKnee));
    addAndMakeVisible(*kneeKnob);

    mixKnob = std::make_unique<ModernKnob>("MIX", ModernLookAndFeel::cyan);
    mixKnob->setParameter(processor.getAPVTS().getParameter(ParamIDs::compMix));
    addAndMakeVisible(*mixKnob);
}

void CompressorPanel::resized()
{
    auto bounds = getLocalBounds().reduced(12);
    bounds.removeFromTop(40); // Header space

    // Style buttons
    auto buttonRow = bounds.removeFromTop(28);
    auto buttonWidth = buttonRow.getWidth() / 4 - 6;
    vcaButton.setBounds(buttonRow.removeFromLeft(buttonWidth));
    buttonRow.removeFromLeft(8);
    fetButton.setBounds(buttonRow.removeFromLeft(buttonWidth));
    buttonRow.removeFromLeft(8);
    optoButton.setBounds(buttonRow.removeFromLeft(buttonWidth));
    buttonRow.removeFromLeft(8);
    varimuButton.setBounds(buttonRow.removeFromLeft(buttonWidth));

    bounds.removeFromTop(12);

    // GR Meter area
    auto meterArea = bounds.removeFromTop(80);
    bounds.removeFromTop(12);

    // Knobs in 3x2 grid
    auto knobHeight = 70;
    auto knobRow1 = bounds.removeFromTop(knobHeight);
    auto knobRow2 = bounds.removeFromTop(knobHeight);

    auto knobWidth = knobRow1.getWidth() / 3;
    thresholdKnob->setBounds(knobRow1.removeFromLeft(knobWidth).reduced(4));
    ratioKnob->setBounds(knobRow1.removeFromLeft(knobWidth).reduced(4));
    attackKnob->setBounds(knobRow1.removeFromLeft(knobWidth).reduced(4));

    releaseKnob->setBounds(knobRow2.removeFromLeft(knobWidth).reduced(4));
    kneeKnob->setBounds(knobRow2.removeFromLeft(knobWidth).reduced(4));
    mixKnob->setBounds(knobRow2.removeFromLeft(knobWidth).reduced(4));
}

void CompressorPanel::paint(juce::Graphics& g)
{
    ModulePanel::paint(g);

    // GR Meter
    auto bounds = getLocalBounds().reduced(12);
    bounds.removeFromTop(40 + 28 + 12); // Skip header and buttons
    auto meterBounds = bounds.removeFromTop(80).reduced(8);

    // Meter background
    g.setColour(ModernLookAndFeel::darkBg);
    g.fillRoundedRectangle(meterBounds.toFloat(), 8.0f);

    // Meter bar
    auto barBounds = meterBounds.reduced(8).removeFromBottom(16);
    g.setColour(ModernLookAndFeel::darkCard);
    g.fillRoundedRectangle(barBounds.toFloat(), 8.0f);

    // GR indicator
    float grAmount = std::abs(processor.getGainReduction()) / 20.0f; // Normalize to 0-1
    if (grAmount > 0.0f)
    {
        auto grWidth = barBounds.getWidth() * juce::jmin(grAmount, 1.0f);
        auto grBar = barBounds.toFloat().removeFromRight(grWidth);
        g.setColour(ModernLookAndFeel::cyan.withAlpha(0.8f));
        g.fillRoundedRectangle(grBar, 8.0f);
    }

    // GR text
    g.setColour(ModernLookAndFeel::cyan);
    g.setFont(juce::Font(10.0f, juce::Font::plain));
    auto textBounds = meterBounds.reduced(8);
    g.drawText(juce::String::formatted("GR: %.1f dB", processor.getGainReduction()),
               textBounds, juce::Justification::centred);
}

// ============================================================================
// ColorPanel
// ============================================================================

ColorPanel::ColorPanel(MultiColorCompProcessor& p)
    : ModulePanel("HARMONIC COLOR", ModernLookAndFeel::amber), processor(p)
{
    // Type buttons
    tapeButton.setButtonText("TAPE");
    tapeButton.setClickingTogglesState(true);
    tapeButton.setRadioGroupId(2);
    tapeButton.setColour(juce::TextButton::buttonOnColourId, ModernLookAndFeel::amber);
    addAndMakeVisible(tapeButton);

    tubeButton.setButtonText("TUBE");
    tubeButton.setClickingTogglesState(true);
    tubeButton.setRadioGroupId(2);
    tubeButton.setColour(juce::TextButton::buttonOnColourId, ModernLookAndFeel::amber);
    addAndMakeVisible(tubeButton);

    transButton.setButtonText("TRANS");
    transButton.setClickingTogglesState(true);
    transButton.setRadioGroupId(2);
    transButton.setColour(juce::TextButton::buttonOnColourId, ModernLookAndFeel::amber);
    addAndMakeVisible(transButton);

    clipButton.setButtonText("CLIP");
    clipButton.setClickingTogglesState(true);
    clipButton.setRadioGroupId(2);
    clipButton.setColour(juce::TextButton::buttonOnColourId, ModernLookAndFeel::amber);
    addAndMakeVisible(clipButton);

    // Create knobs
    driveKnob = std::make_unique<ModernKnob>("DRIVE", ModernLookAndFeel::amber);
    driveKnob->setParameter(processor.getAPVTS().getParameter(ParamIDs::colorDrive));
    addAndMakeVisible(*driveKnob);

    toneKnob = std::make_unique<ModernKnob>("TONE", ModernLookAndFeel::amber);
    toneKnob->setParameter(processor.getAPVTS().getParameter(ParamIDs::colorTone));
    addAndMakeVisible(*toneKnob);

    outputKnob = std::make_unique<ModernKnob>("TRIM", ModernLookAndFeel::amber);
    outputKnob->setParameter(processor.getAPVTS().getParameter(ParamIDs::colorOutput));
    addAndMakeVisible(*outputKnob);

    mixKnob = std::make_unique<ModernKnob>("MIX", ModernLookAndFeel::amber);
    mixKnob->setParameter(processor.getAPVTS().getParameter(ParamIDs::colorMix));
    addAndMakeVisible(*mixKnob);
}

void ColorPanel::resized()
{
    auto bounds = getLocalBounds().reduced(12);
    bounds.removeFromTop(40);

    // Type buttons
    auto buttonRow = bounds.removeFromTop(28);
    auto buttonWidth = buttonRow.getWidth() / 4 - 6;
    tapeButton.setBounds(buttonRow.removeFromLeft(buttonWidth));
    buttonRow.removeFromLeft(8);
    tubeButton.setBounds(buttonRow.removeFromLeft(buttonWidth));
    buttonRow.removeFromLeft(8);
    transButton.setBounds(buttonRow.removeFromLeft(buttonWidth));
    buttonRow.removeFromLeft(8);
    clipButton.setBounds(buttonRow.removeFromLeft(buttonWidth));

    bounds.removeFromTop(12);

    // Visual area
    bounds.removeFromTop(80);
    bounds.removeFromTop(12);

    // Drive knob (centered, larger)
    auto driveArea = bounds.removeFromTop(90);
    driveKnob->setBounds(driveArea.withSizeKeepingCentre(80, 90));

    // Bottom row knobs
    auto knobRow = bounds.removeFromTop(70);
    auto knobWidth = knobRow.getWidth() / 3;
    toneKnob->setBounds(knobRow.removeFromLeft(knobWidth).reduced(4));
    outputKnob->setBounds(knobRow.removeFromLeft(knobWidth).reduced(4));
    mixKnob->setBounds(knobRow.removeFromLeft(knobWidth).reduced(4));
}

void ColorPanel::paint(juce::Graphics& g)
{
    ModulePanel::paint(g);

    // Saturation curve visual
    auto bounds = getLocalBounds().reduced(12);
    bounds.removeFromTop(40 + 28 + 12);
    auto visualBounds = bounds.removeFromTop(80).reduced(8);

    g.setColour(ModernLookAndFeel::darkBg);
    g.fillRoundedRectangle(visualBounds.toFloat(), 8.0f);

    // Draw saturation curve
    juce::Path curve;
    auto curveArea = visualBounds.reduced(12).toFloat();
    float width = curveArea.getWidth();
    float height = curveArea.getHeight();

    curve.startNewSubPath(curveArea.getX(), curveArea.getBottom());
    for (float x = 0; x < width; x += 2.0f)
    {
        float t = x / width;
        float y = std::tanh((t - 0.5f) * 4.0f) * 0.5f + 0.5f;
        curve.lineTo(curveArea.getX() + x, curveArea.getBottom() - y * height);
    }

    g.setColour(ModernLookAndFeel::amber.withAlpha(0.6f));
    g.strokePath(curve, juce::PathStrokeType(2.0f));
}

// ============================================================================
// SoothePanel
// ============================================================================

SoothePanel::SoothePanel(MultiColorCompProcessor& p)
    : ModulePanel("RESONANCE", ModernLookAndFeel::fuchsia), processor(p)
{
    amountKnob = std::make_unique<ModernKnob>("AMOUNT", ModernLookAndFeel::fuchsia);
    amountKnob->setParameter(processor.getAPVTS().getParameter(ParamIDs::sootheAmount));
    addAndMakeVisible(*amountKnob);

    sensitivityKnob = std::make_unique<ModernKnob>("SENS", ModernLookAndFeel::fuchsia);
    sensitivityKnob->setParameter(processor.getAPVTS().getParameter(ParamIDs::sootheSensitivity));
    addAndMakeVisible(*sensitivityKnob);

    sharpnessKnob = std::make_unique<ModernKnob>("SHARP", ModernLookAndFeel::fuchsia);
    sharpnessKnob->setParameter(processor.getAPVTS().getParameter(ParamIDs::sootheSharpness));
    addAndMakeVisible(*sharpnessKnob);

    speedKnob = std::make_unique<ModernKnob>("SPEED", ModernLookAndFeel::fuchsia);
    speedKnob->setParameter(processor.getAPVTS().getParameter(ParamIDs::sootheSpeed));
    addAndMakeVisible(*speedKnob);

    focusLowKnob = std::make_unique<ModernKnob>("FOCUS L", ModernLookAndFeel::fuchsia);
    focusLowKnob->setParameter(processor.getAPVTS().getParameter(ParamIDs::sootheFocusLow));
    addAndMakeVisible(*focusLowKnob);

    focusHighKnob = std::make_unique<ModernKnob>("FOCUS H", ModernLookAndFeel::fuchsia);
    focusHighKnob->setParameter(processor.getAPVTS().getParameter(ParamIDs::sootheFocusHigh));
    addAndMakeVisible(*focusHighKnob);
}

void SoothePanel::resized()
{
    auto bounds = getLocalBounds().reduced(12);
    bounds.removeFromTop(40);

    // Spectral display area
    bounds.removeFromTop(120);
    bounds.removeFromTop(12);

    // Knobs in 3x2 grid
    auto knobHeight = 70;
    auto knobRow1 = bounds.removeFromTop(knobHeight);
    auto knobRow2 = bounds.removeFromTop(knobHeight);

    auto knobWidth = knobRow1.getWidth() / 3;
    amountKnob->setBounds(knobRow1.removeFromLeft(knobWidth).reduced(4));
    sensitivityKnob->setBounds(knobRow1.removeFromLeft(knobWidth).reduced(4));
    sharpnessKnob->setBounds(knobRow1.removeFromLeft(knobWidth).reduced(4));

    speedKnob->setBounds(knobRow2.removeFromLeft(knobWidth).reduced(4));
    focusLowKnob->setBounds(knobRow2.removeFromLeft(knobWidth).reduced(4));
    focusHighKnob->setBounds(knobRow2.removeFromLeft(knobWidth).reduced(4));
}

void SoothePanel::paint(juce::Graphics& g)
{
    ModulePanel::paint(g);

    // Spectral display
    auto bounds = getLocalBounds().reduced(12);
    bounds.removeFromTop(40);
    auto spectrumBounds = bounds.removeFromTop(120).reduced(8);

    g.setColour(ModernLookAndFeel::darkBg);
    g.fillRoundedRectangle(spectrumBounds.toFloat(), 8.0f);

    // Draw grid
    g.setColour(ModernLookAndFeel::darkCard.withAlpha(0.3f));
    for (int i = 1; i < 6; ++i)
    {
        float x = spectrumBounds.getX() + (spectrumBounds.getWidth() * i / 6.0f);
        g.drawVerticalLine((int)x, (float)spectrumBounds.getY(), (float)spectrumBounds.getBottom());
    }
    for (int i = 1; i < 4; ++i)
    {
        float y = spectrumBounds.getY() + (spectrumBounds.getHeight() * i / 4.0f);
        g.drawHorizontalLine((int)y, (float)spectrumBounds.getX(), (float)spectrumBounds.getRight());
    }

    // Frequency labels
    g.setColour(ModernLookAndFeel::textSecondary);
    g.setFont(juce::Font(8.0f));
    auto labelBounds = spectrumBounds.removeFromBottom(12);
    g.drawText("20Hz", labelBounds.removeFromLeft(40), juce::Justification::centredLeft);
    g.drawText("20kHz", labelBounds.removeFromRight(40), juce::Justification::centredRight);
}

// ============================================================================
// MultiColorCompEditor
// ============================================================================

MultiColorCompEditor::MultiColorCompEditor(MultiColorCompProcessor& p)
    : AudioProcessorEditor(&p), processor(p),
      compressorPanel(p), colorPanel(p), soothePanel(p),
      intensityKnob("", ModernLookAndFeel::emerald)
{
    setLookAndFeel(&modernLNF);

    intensityKnob.setParameter(processor.getAPVTS().getParameter(ParamIDs::intensityMacro));
    addAndMakeVisible(intensityKnob);

    routingButton.setButtonText("ROUTE A");
    routingButton.setColour(juce::TextButton::buttonColourId, ModernLookAndFeel::darkCard);
    addAndMakeVisible(routingButton);

    addAndMakeVisible(compressorPanel);
    addAndMakeVisible(colorPanel);
    addAndMakeVisible(soothePanel);

    setSize(1200, 700);
    setResizable(true, true);
    setResizeLimits(1000, 600, 1600, 1000);

    startTimerHz(30);
}

MultiColorCompEditor::~MultiColorCompEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void MultiColorCompEditor::paint(juce::Graphics& g)
{
    g.fillAll(ModernLookAndFeel::darkBg);

    auto bounds = getLocalBounds();

    // Top bar
    auto topBar = bounds.removeFromTop(64);
    g.setColour(ModernLookAndFeel::darkPanel);
    g.fillRect(topBar);

    g.setColour(ModernLookAndFeel::textPrimary);
    g.setFont(juce::Font(18.0f, juce::Font::bold));
    auto titleArea = topBar.removeFromLeft(200).reduced(16, 0);
    g.drawText("MULTI-COLOR", titleArea, juce::Justification::centredLeft);

    g.setColour(ModernLookAndFeel::cyan);
    auto compArea = topBar.removeFromLeft(100);
    g.drawText("COMP", compArea, juce::Justification::centredLeft);

    // Bottom bar
    auto bottomBar = bounds.removeFromBottom(48);
    g.setColour(juce::Colour(0xff0a0f1a));
    g.fillRect(bottomBar);

    // Input/Output meters
    auto meterArea = bottomBar.reduced(16);
    g.setColour(ModernLookAndFeel::textSecondary);
    g.setFont(juce::Font(9.0f));
    g.drawText("INPUT", meterArea.removeFromLeft(50), juce::Justification::centredLeft);

    auto inputMeter = meterArea.removeFromLeft(100).reduced(0, 16);
    g.setColour(ModernLookAndFeel::darkCard);
    g.fillRoundedRectangle(inputMeter.toFloat(), 4.0f);
    float inputLevel = (inputLevelL + inputLevelR) * 0.5f;
    g.setColour(juce::Colour(0xff34d399).withAlpha(0.7f));
    g.fillRoundedRectangle(inputMeter.toFloat().removeFromLeft(inputMeter.getWidth() * juce::jmin(inputLevel, 1.0f)), 4.0f);

    meterArea.removeFromLeft(30);
    g.setColour(ModernLookAndFeel::textSecondary);
    g.drawText("OUTPUT", meterArea.removeFromLeft(50), juce::Justification::centredLeft);

    auto outputMeter = meterArea.removeFromLeft(100).reduced(0, 16);
    g.setColour(ModernLookAndFeel::darkCard);
    g.fillRoundedRectangle(outputMeter.toFloat(), 4.0f);
    float outputLevel = (outputLevelL + outputLevelR) * 0.5f;
    g.setColour(juce::Colour(0xff34d399).withAlpha(0.7f));
    g.fillRoundedRectangle(outputMeter.toFloat().removeFromLeft(outputMeter.getWidth() * juce::jmin(outputLevel, 1.0f)), 4.0f);
}

void MultiColorCompEditor::resized()
{
    auto bounds = getLocalBounds();

    // Top bar
    auto topBar = bounds.removeFromTop(64);
    topBar.removeFromLeft(320); // Skip title area

    // Intensity knob in top bar
    intensityKnob.setBounds(topBar.removeFromRight(90).reduced(8));

    // Routing button
    routingButton.setBounds(topBar.removeFromRight(120).reduced(8, 16));

    // Bottom bar
    bounds.removeFromBottom(48);

    // Module panels
    bounds.reduce(16, 16);
    auto panelWidth = bounds.getWidth() / 3 - 8;

    compressorPanel.setBounds(bounds.removeFromLeft(panelWidth));
    bounds.removeFromLeft(12);
    colorPanel.setBounds(bounds.removeFromLeft(panelWidth));
    bounds.removeFromLeft(12);
    soothePanel.setBounds(bounds);
}

void MultiColorCompEditor::timerCallback()
{
    inputLevelL = processor.getInputLevel(0);
    inputLevelR = processor.getInputLevel(1);
    outputLevelL = processor.getOutputLevel(0);
    outputLevelR = processor.getOutputLevel(1);
    grLevel = processor.getGainReduction();

    compressorPanel.repaint();
    repaint();
}
