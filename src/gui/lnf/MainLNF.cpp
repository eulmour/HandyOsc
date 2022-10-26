#include "MainLNF.hpp"

MainLNF::MainLNF() {
    roboto = juce::Typeface::createSystemTypefaceFor (BinaryData::RobotoCondensedRegular_ttf,
                                                      BinaryData::RobotoCondensedRegular_ttfSize);

    robotoBold = juce::Typeface::createSystemTypefaceFor (BinaryData::RobotoCondensedBold_ttf,
                                                          BinaryData::RobotoCondensedBold_ttfSize);
}

juce::Typeface::Ptr MainLNF::getTypefaceForFont (const juce::Font& font) {
    return font.isBold() ? robotoBold : roboto;
}

/** Positions ComboBox text to leave room for Label */
void MainLNF::positionComboBoxText (juce::ComboBox& box, juce::Label& label) {
    juce::LookAndFeel_V4::positionComboBoxText (box, label);
    label.setFont (label.getFont().boldened());
    label.setJustificationType (juce::Justification::centred);
}

/** Creates tabbed layout for tabbed components */
void MainLNF::createTabTextLayout (
    const juce::TabBarButton& button,
    float length,
    float depth,
    juce::Colour colour,
    juce::TextLayout& textLayout
) {
    juce::Font font (depth * 0.45f, juce::Font::bold);
    font.setUnderline (button.hasKeyboardFocus (false));

    juce::AttributedString s;
    s.setJustification (juce::Justification::centred);
    s.append (button.getButtonText().trim(), font, colour);

    textLayout.createLayout (s, length);
}

/** Finds the proper parent component for a popup menu with these options */
juce::Component* MainLNF::getParentComponentForMenuOptions (const juce::PopupMenu::Options& options) {
#if JUCE_IOS
    if (juce::PluginHostType::getPluginLoadedAs() == juce::AudioProcessor::wrapperType_AudioUnitv3)
    {
        if (options.getParentComponent() == nullptr && options.getTargetComponent() != nullptr)
            return options.getTargetComponent()->getTopLevelComponent();
    }
#endif
    return juce::LookAndFeel_V2::getParentComponentForMenuOptions (options);
}