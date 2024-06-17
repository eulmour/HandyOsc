#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "MainLNF.h"
#include "MiscGroup.h"
#include "TransientGroup.h"

class MainComponent
	: public juce::Component
    , public juce::AudioProcessorParameter::Listener
{
public:
    MainComponent (std::shared_ptr<juce::AudioProcessorValueTreeState> parameters);
    ~MainComponent () override;

    void resized () override;
    void paint (juce::Graphics& g) override;
    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override;

private:

    MainLNF mLnf;
    MiscGroup mMiscGroup;
    TransientGroup mTransientGroup;

    std::shared_ptr<juce::AudioProcessorValueTreeState> mParameters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};