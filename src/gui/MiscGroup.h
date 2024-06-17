#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "Dial.h"

struct MiscGroup: public juce::Component
{
    MiscGroup(std::shared_ptr<juce::AudioProcessorValueTreeState> parameters);
    ~MiscGroup() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

protected:

    struct LookAndFeel : public juce::LookAndFeel_V4
    {
        LookAndFeel() {}
        ~LookAndFeel() override {}
    };

    LookAndFeel mLookAndFeel;
    Dial m_GainDial;
    Dial m_MixDial;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MiscGroup)
};
