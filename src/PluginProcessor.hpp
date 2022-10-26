/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "foleys_gui_magic/foleys_gui_magic.h"

#include "gui/lnf/MainLNF.hpp"
#include "gui/item/DynText.hpp"

#include "dsp/BitReduction.hpp"
#include "dsp/WaveWarp.hpp"
#include "dsp/BiquadFilter.hpp"

//==============================================================================
/**
*/
class ExampleAudioProcessor : public juce::AudioProcessor,
                              private juce::AudioProcessorValueTreeState::Listener
#if JucePlugin_Enable_ARA
                            , public juce::AudioProcessorARAExtension
#endif
{
public:
    //==============================================================================
    ExampleAudioProcessor();
    ~ExampleAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExampleAudioProcessor)
    void parameterChanged (const juce::String& param, float value) override;

protected:
    juce::AudioProcessorValueTreeState vts;
    foleys::MagicProcessorState magicState { *this };

    foleys::MagicPlotSource* oscilloscope = nullptr;

    std::atomic<float>* ioGain;
    std::atomic<float>* ioReductionMix;
    std::atomic<float>* ioWarpMix;

    std::atomic<float>* controlReduction;
    std::atomic<float>* controlWarp;

    std::atomic<float>* filterCutoff;
    std::atomic<float>* filterResonance;

    // toggles
    enum class FilterPlacement {
        Off, Pre, Post
    } filterPlacement = FilterPlacement::Pre;

    float outPeak = 0.f;

//    juce::Label outPeakLabel;

private:
    BitReduction bitReduction;
    WaveWarp waveWarp;
    BiquadFilter biquadFilter;

    std::shared_ptr<MainLNF> mainLNF;
};
