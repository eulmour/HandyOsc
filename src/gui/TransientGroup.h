#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "Dial.h"

struct TransientGroup: public juce::Component
{
    TransientGroup(std::shared_ptr<juce::AudioProcessorValueTreeState> parameters);
    ~TransientGroup() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

protected:

    struct LookAndFeel : public juce::LookAndFeel_V4
    {
        LookAndFeel() {}
        ~LookAndFeel() override {}
    };

    LookAndFeel mLookAndFeel;
    juce::Label mTitleLabel;
    Dial mDecayDial;
    Dial mGainDial;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransientGroup)
};
