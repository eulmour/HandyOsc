#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "EditableParameterComponent.h"

class Dial : public juce::Component
{

public:
    Dial (std::string const& labelText,
          std::shared_ptr<juce::AudioProcessorValueTreeState> parameters,
          std::string const& parameterId);
    ~Dial() override;

    void resized() override;

protected:

    struct LookAndFeel : public juce::LookAndFeel_V4
    {
        LookAndFeel();
        ~LookAndFeel() override;
        juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override;
        void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;
        void drawBorder(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider);
        void drawCircle(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider);
        void drawStick(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider);
    };

    LookAndFeel mLookAndFeel;
    juce::Slider mSlider;
    juce::Label mLabel;
    EditableParameterComponent mDialValue;
    juce::AudioProcessorValueTreeState::SliderAttachment mSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dial)
private:
    void updateFontSize();
};