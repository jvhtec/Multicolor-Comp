#include "PluginProcessor.h"
#include "PluginEditor.h"

MultiColorCompProcessor::MultiColorCompProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this)
{
}

MultiColorCompProcessor::~MultiColorCompProcessor()
{
}

void MultiColorCompProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    router.prepare(spec);
}

void MultiColorCompProcessor::releaseResources()
{
    router.reset();
}

bool MultiColorCompProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Support mono and stereo
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void MultiColorCompProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    // Input metering
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        inputLevel[ch].store(buffer.getRMSLevel(ch, 0, buffer.getNumSamples()));
    }

    // Process audio through router
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    router.process(context, parameters);

    // Get gain reduction for metering
    gainReduction.store(router.getGainReduction());

    // Output metering
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        outputLevel[ch].store(buffer.getRMSLevel(ch, 0, buffer.getNumSamples()));
    }
}

juce::AudioProcessorEditor* MultiColorCompProcessor::createEditor()
{
    return new MultiColorCompEditor(*this);
}

void MultiColorCompProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.getAPVTS().copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MultiColorCompProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(parameters.getAPVTS().state.getType()))
        {
            parameters.getAPVTS().replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MultiColorCompProcessor();
}
