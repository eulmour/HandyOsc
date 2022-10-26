#pragma once
#include "JuceHeader.h"
#include "BinaryData.h"

class MainLNF : public juce::LookAndFeel_V4 {
public:
    MainLNF();

    juce::Typeface::Ptr getTypefaceForFont (const juce::Font&) override;

    /** Positions ComboBox text to leave room for Label */
    void positionComboBoxText (juce::ComboBox& box, juce::Label& label) override;

    /** Creates tabbed layout for tabbed components */
    void createTabTextLayout (
        const juce::TabBarButton& button,
        float length,
        float depth,
        juce::Colour colour,
        juce::TextLayout& textLayout);

    /** Finds the proper parent component for a popup menu with these options */
    juce::Component* getParentComponentForMenuOptions (const juce::PopupMenu::Options& options) override;

protected:
    std::unique_ptr<juce::Drawable> knob = juce::Drawable::createFromImageData (
        BinaryData::knob_svg, BinaryData::knob_svgSize);

    std::unique_ptr<juce::Drawable> pointer = juce::Drawable::createFromImageData (
        BinaryData::pointer_svg, BinaryData::pointer_svgSize);

    juce::Typeface::Ptr roboto;
    juce::Typeface::Ptr robotoBold;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainLNF)
};