#include "RouterModule.h"

RouterModule::RouterModule()
{
}

void RouterModule::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    compressor.prepare(spec);
    color.prepare(spec);
    soothe.prepare(spec);

    inputGain.prepare(spec);
    outputGain.prepare(spec);

    reset();
}

void RouterModule::reset()
{
    compressor.reset();
    color.reset();
    soothe.reset();

    inputGain.reset();
    outputGain.reset();
}

void RouterModule::process(juce::dsp::ProcessContextReplacing<float>& context, Parameters& params)
{
    auto block = context.getOutputBlock();

    // Input trim
    const float inputTrimDB = params.getValue(ParamIDs::inputTrim);
    inputGain.setGainDecibels(inputTrimDB);
    inputGain.process(context);

    // Route selection
    const int routing = params.getIntValue(ParamIDs::routing);

    if (routing == 0)
    {
        processRouteA(block, params);
    }
    else
    {
        processRouteB(block, params);
    }

    // Output trim
    const float outputTrimDB = params.getValue(ParamIDs::outputTrim);
    outputGain.setGainDecibels(outputTrimDB);
    outputGain.process(context);

    // Global mix (if needed)
    // For now, individual modules handle their own mix
}

void RouterModule::processRouteA(juce::dsp::AudioBlock<float>& block, Parameters& params)
{
    // Route A: Soothe → Compressor → Color
    soothe.process(block, params);
    compressor.process(block, params);
    color.process(block, params);
}

void RouterModule::processRouteB(juce::dsp::AudioBlock<float>& block, Parameters& params)
{
    // Route B: Compressor → Color → Soothe
    compressor.process(block, params);
    color.process(block, params);
    soothe.process(block, params);
}

int RouterModule::getLatencySamples() const
{
    // Report latency from Soothe module
    return soothe.getLatencySamples();
}
