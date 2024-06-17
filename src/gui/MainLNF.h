#pragma once

#include "JuceHeader.h"
#include "BinaryData.h"

class MainLNF : public juce::LookAndFeel_V4 {

public:
    static constexpr int FontScaleMedium{ 90 };
    static constexpr int FontScaleLarge{ 80 };

    static inline const juce::Colour BackgroundPrimary = juce::Colour(30, 30, 30);
    static inline const juce::Colour BackgroundSecondary = juce::Colour(50, 50, 50);
    static inline const juce::Colour BackgroundTertiary = juce::Colour(42, 42, 42);
    static inline const juce::Colour ForegroundPrimary = juce::Colour(47, 132, 175);
    static inline const juce::Colour ForegroundSecondary = juce::Colour(171, 14, 54);
    static inline const juce::Colour ForegroundTertiary = juce::Colour(28, 212, 184);
    static inline const juce::Colour Warning = juce::Colour(255, 214, 0);
    static inline const juce::Colour TextFocusLevel0 = juce::Colour(255, 255, 255);
    static inline const juce::Colour TextFocusLevel1 = juce::Colour(255, 255, 255).withAlpha(0.70f);
    static inline const juce::Colour TextFocusLevel2 = juce::Colour(255, 255, 255).withAlpha(0.50f);
    static inline const juce::Colour TextFocusLevel3 = juce::Colour(255, 255, 255).withAlpha(0.38f);

    MainLNF();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainLNF)
};
