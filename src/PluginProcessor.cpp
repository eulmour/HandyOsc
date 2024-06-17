/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace IDs {
juce::String miscMix { "Mix" };
juce::String miscGain { "OutputGain" };

juce::String transientDecay { "TransientDecay" };
juce::String transientGain { "TransientGain" };
} // namespace IDs

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    auto miscGroup = std::make_unique<juce::AudioProcessorParameterGroup> ("miscGroup", TRANS ("Misc"), "|");
    miscGroup->addChild (
        std::make_unique<juce::AudioParameterFloat> (IDs::miscMix, "Mix", juce::NormalisableRange<float> (0.0f, 1.0f), 1.0f),
        std::make_unique<juce::AudioParameterFloat> (IDs::miscGain, "Output Gain", juce::NormalisableRange<float> (0.f, 2.f), 1.0f)
    );

    auto transientGroup = std::make_unique<juce::AudioProcessorParameterGroup> ("transientGroup", TRANS ("Transient"), "|");
    transientGroup->addChild (
        std::make_unique<juce::AudioParameterFloat> (IDs::transientDecay, "Transient Decay", juce::NormalisableRange<float> (0.0f, 100.0f), 1.0f),
        std::make_unique<juce::AudioParameterFloat> (IDs::transientGain, "Transient Gain", juce::NormalisableRange<float> (0.f, 24.f), 0.0f)
    );

    layout.add(std::move (miscGroup), std::move(transientGroup));

    return layout;
}

//==============================================================================
WarpRoomAudioProcessor::WarpRoomAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput ("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
    , mParameters(std::make_shared<juce::AudioProcessorValueTreeState>(*this, nullptr, juce::Identifier(JucePlugin_Name), createParameterLayout()))
#endif
{
    this->mMiscMix = mParameters->getRawParameterValue (IDs::miscMix);
    this->mMiscGain = mParameters->getRawParameterValue (IDs::miscGain);
    jassert (this->mMiscMix != nullptr);
    jassert (this->mMiscGain != nullptr);

    mParameters->addParameterListener (IDs::miscMix, this);
    mParameters->addParameterListener (IDs::miscGain, this);
}

WarpRoomAudioProcessor::~WarpRoomAudioProcessor() = default;

//==============================================================================
const juce::String WarpRoomAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WarpRoomAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool WarpRoomAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool WarpRoomAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double WarpRoomAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WarpRoomAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
}

int WarpRoomAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WarpRoomAudioProcessor::setCurrentProgram (int index)
{
    (void)index;
}

const juce::String WarpRoomAudioProcessor::getProgramName (int index)
{
    (void)index;
    return {};
}

void WarpRoomAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    (void)index;
    (void)newName;
}

//==============================================================================
void WarpRoomAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void WarpRoomAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WarpRoomAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void WarpRoomAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // juce::dsp::AudioBlock<float> inBuffer(buffer);
    juce::dsp::AudioBlock<float> outBuffer(buffer);

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto sample = buffer.getWritePointer (channel);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            sample[i] *= this->mMiscGain->load();
        }
    }
}

//==============================================================================
bool WarpRoomAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WarpRoomAudioProcessor::createEditor()
{
    return new WarpRoomEditor(*this, this->mParameters);
}

//==============================================================================
void WarpRoomAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    (void)destData;
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void WarpRoomAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    (void)data;
    (void)sizeInBytes;
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void WarpRoomAudioProcessor::parameterChanged (const juce::String& param, float value)
{
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WarpRoomAudioProcessor();
}
