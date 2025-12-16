cmake_minimum_required(VERSION 3.22)

# Basic test executable
add_executable(CompressorTest
    CompressorTest.cpp
    ${CMAKE_SOURCE_DIR}/src/Parameters.cpp
    ${CMAKE_SOURCE_DIR}/src/dsp/CompressorModule.cpp
    ${CMAKE_SOURCE_DIR}/src/dsp/ColorModule.cpp
    ${CMAKE_SOURCE_DIR}/src/dsp/SootheModule.cpp
)

target_include_directories(CompressorTest PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/dsp
)

target_link_libraries(CompressorTest PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)

target_compile_definitions(CompressorTest PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

# Add test
add_test(NAME CompressorTest COMMAND CompressorTest)
