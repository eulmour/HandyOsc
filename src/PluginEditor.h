#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "gui/MainComponent.h"

class WarpRoomEditor  : public juce::AudioProcessorEditor
{
public:
    explicit WarpRoomEditor (WarpRoomAudioProcessor&,
        std::shared_ptr<juce::AudioProcessorValueTreeState> parameters);

    ~WarpRoomEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WarpRoomAudioProcessor& processorRef;

    MainComponent mMainComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WarpRoomEditor)
};