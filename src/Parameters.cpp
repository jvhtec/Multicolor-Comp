#include "Parameters.h"

Parameters::Parameters(juce::AudioProcessor& processor)
    : apvts(processor, nullptr, "Parameters", createParameterLayout())
{
}

float Parameters::getValue(const juce::String& paramID) const
{
    if (auto* param = apvts.getRawParameterValue(paramID))
        return param->load();
    return 0.0f;
}

int Parameters::getIntValue(const juce::String& paramID) const
{
    return static_cast<int>(getValue(paramID));
}

bool Parameters::getBoolValue(const juce::String& paramID) const
{
    return getValue(paramID) > 0.5f;
}

juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Global
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::inputTrim, 1}, "Input Trim",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::outputTrim, 1}, "Output Trim",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::globalMix, 1}, "Global Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ParamIDs::routing, 1}, "Routing",
        juce::StringArray{"Soothe->Comp->Color", "Comp->Color->Soothe"}, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::intensityMacro, 1}, "Intensity",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Compressor
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ParamIDs::compBypass, 1}, "Comp Bypass", false));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ParamIDs::compStyle, 1}, "Comp Style",
        juce::StringArray{"VCA", "FET", "Opto", "Vari-Mu"}, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::compThreshold, 1}, "Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -10.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::compRatio, 1}, "Ratio",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f, 0.4f), 2.5f,
        juce::AudioParameterFloatAttributes().withLabel(":1")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::compAttack, 1}, "Attack",
        juce::NormalisableRange<float>(0.1f, 50.0f, 0.1f, 0.3f), 10.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::compRelease, 1}, "Release",
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.4f), 120.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::compKnee, 1}, "Knee",
        juce::NormalisableRange<float>(0.0f, 12.0f, 0.1f), 6.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::compMakeup, 1}, "Makeup",
        juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::compMix, 1}, "Comp Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::compSCHPF, 1}, "SC HPF",
        juce::NormalisableRange<float>(20.0f, 200.0f, 1.0f, 0.3f), 80.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ParamIDs::compStereoLink, 1}, "Stereo Link",
        juce::StringArray{"Dual Mono", "Average", "Max"}, 1));

    // Color
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ParamIDs::colorBypass, 1}, "Color Bypass", false));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ParamIDs::colorType, 1}, "Color Type",
        juce::StringArray{"Tape", "Tube", "Transformer", "Clip"}, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::colorDrive, 1}, "Drive",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 25.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::colorTone, 1}, "Tone",
        juce::NormalisableRange<float>(-50.0f, 50.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::colorMix, 1}, "Color Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::colorOutput, 1}, "Color Output",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ParamIDs::colorOS, 1}, "Oversampling",
        juce::StringArray{"Auto", "Off", "2x", "4x", "8x"}, 0));

    // Soothe
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ParamIDs::sootheBypass, 1}, "Soothe Bypass", true));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::sootheAmount, 1}, "Soothe Amount",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::sootheSensitivity, 1}, "Sensitivity",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::sootheSharpness, 1}, "Sharpness",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::sootheSpeed, 1}, "Speed",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::sootheFocusLow, 1}, "Focus Low",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f), 20.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::sootheFocusHigh, 1}, "Focus High",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f), 20000.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::sootheMix, 1}, "Soothe Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ParamIDs::sootheDelta, 1}, "Delta", false));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ParamIDs::sootheQuality, 1}, "Quality",
        juce::StringArray{"Eco", "Normal", "High"}, 1));

    return layout;
}
