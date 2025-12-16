cmake_minimum_required(VERSION 3.22)

project(MultiColorComp VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Fetch JUCE
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.12
)
FetchContent_MakeAvailable(JUCE)

# Plugin formats to build
set(FORMATS VST3)

# Main plugin target
juce_add_plugin(MultiColorComp
    COMPANY_NAME "YourCompany"
    PLUGIN_MANUFACTURER_CODE Ycmp
    PLUGIN_CODE Mcmp
    FORMATS ${FORMATS}
    PRODUCT_NAME "Multi-Color Comp"
    BUNDLE_ID "com.yourcompany.multicolorcomp"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_CATEGORIES Fx Dynamics Distortion
)

# Source files
target_sources(MultiColorComp PRIVATE
    src/PluginProcessor.cpp
    src/PluginEditor.cpp
    src/Parameters.cpp
    src/dsp/Smoothing.cpp
    src/dsp/CompressorModule.cpp
    src/dsp/ColorModule.cpp
    src/dsp/SootheModule.cpp
    src/dsp/RouterModule.cpp
    src/ui/ModernLookAndFeel.cpp
    src/ui/ModernKnob.cpp
)

# Include directories
target_include_directories(MultiColorComp PRIVATE
    src
    src/dsp
    src/ui
)

# JUCE modules
target_compile_definitions(MultiColorComp PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
)

target_link_libraries(MultiColorComp PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# Tests
enable_testing()
add_subdirectory(tests)
