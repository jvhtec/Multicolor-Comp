#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

// Parameter IDs as constants
namespace ParamIDs
{
    // Global
    inline constexpr auto inputTrim = "input_trim";
    inline constexpr auto outputTrim = "output_trim";
    inline constexpr auto globalMix = "global_mix";
    inline constexpr auto routing = "routing";  // 0=Soothe->Comp->Color, 1=Comp->Color->Soothe
    inline constexpr auto intensityMacro = "intensity_macro";

    // Compressor
    inline constexpr auto compBypass = "comp_bypass";
    inline constexpr auto compStyle = "comp_style";  // 0=VCA, 1=FET, 2=Opto, 3=VariMu
    inline constexpr auto compThreshold = "comp_threshold";
    inline constexpr auto compRatio = "comp_ratio";
    inline constexpr auto compAttack = "comp_attack";
    inline constexpr auto compRelease = "comp_release";
    inline constexpr auto compKnee = "comp_knee";
    inline constexpr auto compMakeup = "comp_makeup";
    inline constexpr auto compMix = "comp_mix";
    inline constexpr auto compSCHPF = "comp_sc_hpf";
    inline constexpr auto compStereoLink = "comp_stereo_link";  // 0=DualMono, 1=Average, 2=Max

    // Color
    inline constexpr auto colorBypass = "color_bypass";
    inline constexpr auto colorType = "color_type";  // 0=Tape, 1=Tube, 2=Transformer, 3=Clip
    inline constexpr auto colorDrive = "color_drive";
    inline constexpr auto colorTone = "color_tone";
    inline constexpr auto colorMix = "color_mix";
    inline constexpr auto colorOutput = "color_output";
    inline constexpr auto colorOS = "color_os";  // 0=Auto, 1=Off, 2=2x, 3=4x, 4=8x

    // Soothe
    inline constexpr auto sootheBypass = "soothe_bypass";
    inline constexpr auto sootheAmount = "soothe_amount";
    inline constexpr auto sootheSensitivity = "soothe_sensitivity";
    inline constexpr auto sootheSharpness = "soothe_sharpness";
    inline constexpr auto sootheSpeed = "soothe_speed";
    inline constexpr auto sootheFocusLow = "soothe_focus_low";
    inline constexpr auto sootheFocusHigh = "soothe_focus_high";
    inline constexpr auto sootheMix = "soothe_mix";
    inline constexpr auto sootheDelta = "soothe_delta";
    inline constexpr auto sootheQuality = "soothe_quality";  // 0=Eco, 1=Normal, 2=High
}

class Parameters
{
public:
    Parameters(juce::AudioProcessor& processor);

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // Helper to get parameter value
    float getValue(const juce::String& paramID) const;
    int getIntValue(const juce::String& paramID) const;
    bool getBoolValue(const juce::String& paramID) const;

private:
    juce::AudioProcessorValueTreeState apvts;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
};
