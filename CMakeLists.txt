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
# AAX requires AAX_SDK_PATH to be set
if(DEFINED AAX_SDK_PATH)
    set(FORMATS VST3 AU AAX Standalone)
    message(STATUS "Building with AAX support (AAX SDK found at ${AAX_SDK_PATH})")
else()
    set(FORMATS VST3 AU Standalone)
    message(STATUS "Building without AAX (set AAX_SDK_PATH to enable)")
endif()

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
    AU_MAIN_TYPE kAudioUnitType_Effect
    AAX_CATEGORY AAX_ePlugInCategory_Dynamics
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

# Platform-specific settings
if(APPLE)
    # macOS deployment target
    set_target_properties(MultiColorComp PROPERTIES
        XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.13"
        XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++20"
        XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
    )

    # Code signing (optional, can be set via environment)
    if(DEFINED ENV{APPLE_SIGNING_IDENTITY})
        set_target_properties(MultiColorComp PROPERTIES
            XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY $ENV{APPLE_SIGNING_IDENTITY}
        )
    endif()
elseif(WIN32)
    # Windows-specific settings
    target_compile_definitions(MultiColorComp PRIVATE
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
    )

    # Use static runtime for better compatibility
    if(MSVC)
        set_property(TARGET MultiColorComp PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

# AAX SDK path configuration
if(DEFINED AAX_SDK_PATH)
    juce_set_aax_sdk_path(${AAX_SDK_PATH})
endif()

# Tests
enable_testing()
add_subdirectory(tests)
