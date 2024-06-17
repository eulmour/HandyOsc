#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class EditableParameterComponent : public juce::Component
{
public:
    EditableParameterComponent (
        std::shared_ptr<juce::AudioProcessorValueTreeState> parameters,
        std::string const& parameterId);
    ~EditableParameterComponent() override;

    void resized() override;
    void setSuffix (std::string const& suffix);
    void mouseDown (juce::MouseEvent const&) override;

private:
    struct Attachment : public juce::ParameterAttachment
    {
        Attachment (juce::RangedAudioParameter& parameter, juce::Label& labelComponent);
        ~Attachment() override;
    private:
        void onParameterChanged (float newValue);
        void onLabelTextChanged();
        juce::RangedAudioParameter& mParameter;
        juce::Label& mLabelComponent;
    };

    struct LookAndFeel : public juce::LookAndFeel_V4
    {
        void drawTextEditorOutline (juce::Graphics&, int, int, juce::TextEditor&) override
        {
            // intentionally leave blank to suppress border drawing
        }
    };

    juce::Label mValueLabel;
    juce::Label mSuffixLabel;
    LookAndFeel mValueLabelLookAndFeel;
    Attachment mValueAttachment;

    void updateFontSize();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EditableParameterComponent)
};