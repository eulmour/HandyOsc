/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.hpp"

namespace IDs {

static juce::String ioGain { "ioGain" };
static juce::String ioPeak { "ioPeak" };
static juce::String ioReductionMix { "ioReductionMix" };
static juce::String ioWarpMix { "ioWarpMix" };

static juce::String controlReduction { "controlReduction" };
static juce::String controlWarp { "controlWarp" };
static juce::String controlLogicType { "controlLogicType" };
static juce::String filterCutoff{ "filterCutoff" };
static juce::String filterResonance{ "filterResonance" };
static juce::String filterPlace{ "filterPlace" };
static juce::Identifier oscilloscope { "oscilloscope" };

} // namespace IDs

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    auto controlGroup = std::make_unique<juce::AudioProcessorParameterGroup> ("controlGroup", TRANS ("Control"), "|");
    controlGroup->addChild (
        std::make_unique<juce::AudioParameterChoice> (IDs::controlLogicType, "Warp Type", juce::StringArray("A", "B", "C"), 0),
        std::make_unique<juce::AudioParameterInt> (IDs::controlReduction, "Reduction", 1, 16, 16),
        std::make_unique<juce::AudioParameterFloat> (IDs::controlWarp, "Warp", juce::NormalisableRange<float> (0.f, 1.f, 0.f, 0.25f), 0.005f)
    );

    auto filterGroup = std::make_unique<juce::AudioProcessorParameterGroup> ("filterGroup", TRANS ("Filter"), "|");
    filterGroup->addChild (
        std::make_unique<juce::AudioParameterChoice> (IDs::filterPlace, "Placement", juce::StringArray("Off", "Pre", "Post"), 1),
        std::make_unique<juce::AudioParameterFloat> (IDs::filterCutoff, "Cutoff", juce::NormalisableRange<float> (20.f, 20000.f, 0.f, .25f), 20000.f),
        std::make_unique<juce::AudioParameterFloat> (IDs::filterResonance, "Resonance", juce::NormalisableRange<float> (0.1f, 96.f, 0.f, .25f), .1f)
    );

    auto ioGroup = std::make_unique<juce::AudioProcessorParameterGroup> ("ioGroup", TRANS ("I/O"), "|");
    ioGroup->addChild (
        std::make_unique<juce::AudioParameterFloat> (IDs::ioGain, "Gain", juce::NormalisableRange<float> (0.f, 2.f), 1.0f),
        std::make_unique<juce::AudioParameterFloat> (IDs::ioReductionMix, "Reduction Mix", juce::NormalisableRange<float> (0.f, 1.f), 1.0f),
        std::make_unique<juce::AudioParameterFloat> (IDs::ioWarpMix, "Warp Mix", juce::NormalisableRange<float> (0.f, 1.f), 1.0f)
    );

    layout.add (std::move (controlGroup), std::move(filterGroup), std::move (ioGroup));

    return layout;
}

//==============================================================================
ExampleAudioProcessor::ExampleAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput ("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
    , vts (*this, nullptr, juce::Identifier ("Parameters"), createParameterLayout()),
      biquadFilter(3000.f, 1.f)
#endif
{
    this->controlReduction = vts.getRawParameterValue (IDs::controlReduction);
    jassert (this->controlReduction != nullptr);

    this->controlWarp = vts.getRawParameterValue (IDs::controlWarp);
    jassert (this->controlWarp != nullptr);

    this->filterCutoff = vts.getRawParameterValue (IDs::filterCutoff);
    jassert (this->filterCutoff != nullptr);

    this->filterResonance = vts.getRawParameterValue (IDs::filterResonance);
    jassert (this->filterResonance != nullptr);

    this->ioGain = vts.getRawParameterValue (IDs::ioGain);
    jassert (this->ioGain != nullptr);

    this->ioReductionMix = vts.getRawParameterValue (IDs::ioReductionMix);
    jassert (this->ioReductionMix != nullptr);

    this->ioWarpMix = vts.getRawParameterValue (IDs::ioWarpMix);
    jassert (this->ioWarpMix != nullptr);

    this->waveWarp.type = vts.getRawParameterValue (IDs::controlLogicType);
    jassert (this->waveWarp.type != nullptr);

    vts.addParameterListener (IDs::controlReduction, this);
    vts.addParameterListener (IDs::controlWarp, this);
    vts.addParameterListener (IDs::filterCutoff, this);
    vts.addParameterListener (IDs::filterResonance, this);
    vts.addParameterListener (IDs::filterPlace, this);
    vts.addParameterListener (IDs::ioGain, this);
    vts.addParameterListener (IDs::ioReductionMix, this);
    vts.addParameterListener (IDs::ioWarpMix, this);

    oscilloscope = magicState.createAndAddObject<foleys::MagicOscilloscope> (IDs::oscilloscope, 0);
    // peakInfo = magicState.createAndAddObject<foleys::MagicOscilloscope> (IDs::oscilloscope, 0);
    magicState.setGuiValueTree (BinaryData::gui_xml, BinaryData::gui_xmlSize);
}

ExampleAudioProcessor::~ExampleAudioProcessor() = default;

//==============================================================================
const juce::String ExampleAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ExampleAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool ExampleAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool ExampleAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double ExampleAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ExampleAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
}

int ExampleAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ExampleAudioProcessor::setCurrentProgram (int index)
{
    (void)index;
}

const juce::String ExampleAudioProcessor::getProgramName (int index)
{
    (void)index;
    return {};
}

void ExampleAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    (void)index;
    (void)newName;
}

//==============================================================================
void ExampleAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    this->biquadFilter.setSampleRate(sampleRate);

    // MAGIC GUI: this will setup all internals like MagicPlotSources etc.
    magicState.prepareToPlay (sampleRate, samplesPerBlock);
}

void ExampleAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ExampleAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ExampleAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
    switch (this->filterPlacement) {
        case FilterPlacement::Pre:
            this->biquadFilter.process<juce::dsp::ProcessContextReplacing<float>>(outBuffer);

            this->bitReduction.process<juce::dsp::ProcessContextReplacing<float>>(
                outBuffer, this->controlReduction->load());

            this->waveWarp.process<juce::dsp::ProcessContextReplacing<float>>(
                outBuffer, this->controlWarp->load());
            break;
        case FilterPlacement::Post:
            this->bitReduction.process<juce::dsp::ProcessContextReplacing<float>>(
                outBuffer, this->controlReduction->load());

            this->waveWarp.process<juce::dsp::ProcessContextReplacing<float>>(
                outBuffer, this->controlWarp->load());

            this->biquadFilter.process<juce::dsp::ProcessContextReplacing<float>>(outBuffer);
            break;
        default:
            this->bitReduction.process<juce::dsp::ProcessContextReplacing<float>>(
                outBuffer, this->controlReduction->load());

            this->waveWarp.process<juce::dsp::ProcessContextReplacing<float>>(
                outBuffer, this->controlWarp->load());
    }

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto sample = buffer.getWritePointer (channel);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            sample[i] *= this->ioGain->load();
        }
    }

    // magicState.getGuiTree().
    oscilloscope->pushSamples(buffer);
}

//==============================================================================
bool ExampleAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

// juce::SharedResourcePointer<LNFAllocator>;
juce::AudioProcessorEditor* ExampleAudioProcessor::createEditor()
{
    this->mainLNF = std::make_shared<MainLNF>();

    // Register GUI items for Foleys GUI Magic
    auto builder = std::make_unique<foleys::MagicGUIBuilder> (magicState);

    builder->registerJUCEFactories();
    builder->registerJUCELookAndFeels();

    builder->registerLookAndFeel("MainLNF", std::make_unique<MainLNF>());
    LookAndFeel::setDefaultLookAndFeel(this->mainLNF.get());

    builder->registerFactory("DynText", &DynTextItem::factory);

    auto editor = new foleys::MagicPluginEditor (magicState, std::move (builder));

    editor->setResizeLimits (260, 500, 520, 800);
    editor->setSize (260, 500);
    return editor;
}

//==============================================================================
void ExampleAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    (void)destData;
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ExampleAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    (void)data;
    (void)sizeInBytes;
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void ExampleAudioProcessor::parameterChanged (const juce::String& param, float value)
{
    if (param == IDs::filterCutoff) {
        biquadFilter.setFrequency(value);
    } else if (param == IDs::filterResonance) {
        biquadFilter.setResonance (value);
    } else if (param == IDs::filterPlace) {
        switch (juce::roundToInt(value)) {
            case 1: this->filterPlacement = FilterPlacement::Pre; break;
            case 2: this->filterPlacement = FilterPlacement::Post; break;
            default: this->filterPlacement = FilterPlacement::Off; break;
        }
    } else if (param == IDs::ioReductionMix) {
        this->bitReduction.setMixAmount(value);
    } else if (param == IDs::ioWarpMix) {
        this->waveWarp.setMixAmount(value);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ExampleAudioProcessor();
}
