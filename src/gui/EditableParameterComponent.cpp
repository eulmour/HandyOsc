#include "EditableParameterComponent.h"
#include <juce_graphics/juce_graphics.h>
#include "MainLNF.h"

EditableParameterComponent::EditableParameterComponent (
    std::shared_ptr<juce::AudioProcessorValueTreeState> parameters,
    std::string const& parameterId)
    : mValueAttachment (*parameters->getParameter (parameterId), mValueLabel)
{
    mValueLabel.setEditable (true);
    mValueLabel.setBorderSize (juce::BorderSize<int> (0));
    mValueLabel.onEditorShow = std::bind (&EditableParameterComponent::resized, this);
    mValueLabel.onEditorHide = std::bind (&EditableParameterComponent::resized, this);
    mValueLabel.setColour (juce::Label::textColourId, juce::Colours::black);
    mValueLabel.setLookAndFeel (&mValueLabelLookAndFeel);
    addAndMakeVisible (mValueLabel);

    mSuffixLabel.setBorderSize (juce::BorderSize<int> (0));
    mSuffixLabel.addMouseListener (this, false);
    mSuffixLabel.setColour (juce::Label::textColourId, juce::Colours::black);
    mSuffixLabel.setColour (juce::Label::textWhenEditingColourId, juce::Colours::black);
    addAndMakeVisible (mSuffixLabel);

    updateFontSize();
}

EditableParameterComponent::~EditableParameterComponent()
{
    mSuffixLabel.removeMouseListener (this);
    mValueLabel.setLookAndFeel (nullptr);
    mSuffixLabel.setLookAndFeel (nullptr);
    setLookAndFeel (nullptr);
}

void EditableParameterComponent::resized()
{
    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    using Item = juce::GridItem;
    grid.templateRows = { Track (Fr (1)) };
    if (mValueLabel.isBeingEdited())
    {
        grid.templateColumns = { Track (Fr (1)) };
        grid.items = { Item (mValueLabel) };
        mValueLabel.setJustificationType (juce::Justification::centred);
        mSuffixLabel.setVisible (false);
    }
    else if (mSuffixLabel.getText().isNotEmpty())
    {
        grid.templateColumns = { Track (Fr (1)), Track (Fr (1)) };
        grid.items = { Item (mValueLabel), Item (mSuffixLabel) };
        mValueLabel.setJustificationType (juce::Justification::right);
        mSuffixLabel.setJustificationType (juce::Justification::left);
        mSuffixLabel.setVisible (true);
    }
    else
    {
        grid.templateColumns = { Track (Fr (1)) };
        grid.items = { Item (mValueLabel) };
        mValueLabel.setJustificationType (juce::Justification::centred);
        mSuffixLabel.setVisible (false);
    }
    grid.performLayout (getLocalBounds());
    updateFontSize();
}

void EditableParameterComponent::setSuffix (std::string const& suffix)
{
    mSuffixLabel.setText (suffix, juce::NotificationType::dontSendNotification);
}

void EditableParameterComponent::updateFontSize()
{
    auto const topLevelBounds = getTopLevelComponent()->getBounds();
    auto const topLevelWidth = topLevelBounds.getWidth();
    auto const topLevelHeight = topLevelBounds.getHeight();

    auto const fontSize = (topLevelWidth + topLevelHeight) / MainLNF::FontScaleMedium;

    mValueLabel.setFont (fontSize);
    mSuffixLabel.setFont (fontSize);
    if (auto editor = mValueLabel.getCurrentTextEditor())
    {
        editor->setJustification (juce::Justification::centred);
        editor->setBorder (juce::BorderSize<int> (0));
    }
}

void EditableParameterComponent::mouseDown (juce::MouseEvent const&)
{
    if (! mValueLabel.isBeingEdited())
    {
        mValueLabel.showEditor();
    }
}

EditableParameterComponent::Attachment::Attachment (juce::RangedAudioParameter& parameter, juce::Label& labelComponent)
	: juce::ParameterAttachment (parameter, std::bind (&EditableParameterComponent::Attachment::onParameterChanged, this, std::placeholders::_1))
	, mParameter (parameter)
	, mLabelComponent (labelComponent)
{
	mLabelComponent.onTextChange = std::bind (&EditableParameterComponent::Attachment::onLabelTextChanged, this);
	sendInitialUpdate();
}

EditableParameterComponent::Attachment::~Attachment()
{
}

void EditableParameterComponent::Attachment::onParameterChanged (float newValue)
{
	auto const normalised = mParameter.convertTo0to1 (newValue);
	mLabelComponent.setText (mParameter.getText (normalised, 4), juce::dontSendNotification);
}

void EditableParameterComponent::Attachment::onLabelTextChanged()
{
	auto const labelText = mLabelComponent.getText();
	auto const denormalised = mParameter.convertFrom0to1 (mParameter.getValueForText (labelText));
	setValueAsCompleteGesture (denormalised);
}