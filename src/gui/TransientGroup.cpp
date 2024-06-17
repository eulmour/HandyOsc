#include "TransientGroup.h"

TransientGroup::TransientGroup (std::shared_ptr<juce::AudioProcessorValueTreeState> parameters)
    : mDecayDial ("Decay", parameters, "TransientDecay")
    , mGainDial ("Gain", parameters, "TransientGain")
{
    addAndMakeVisible (mDecayDial);
    addAndMakeVisible (mGainDial);
    addAndMakeVisible (mTitleLabel);

	mTitleLabel.setFont (11.0f);
    mTitleLabel.setText (juce::String ("Transient"), juce::dontSendNotification);
	mTitleLabel.setJustificationType (juce::Justification::centredTop);
	mTitleLabel.setColour (juce::Label::textColourId, juce::Colours::black);

    setLookAndFeel(&mLookAndFeel);
}

TransientGroup::~TransientGroup()
{
    setLookAndFeel(nullptr);
	mTitleLabel.setLookAndFeel (nullptr);
}

void TransientGroup::paint (juce::Graphics& g)
{
	float thickness = 2.0f;
    constexpr float corner = 10.0f;
    const auto bounds = getLocalBounds().toFloat().reduced (thickness);

    g.setColour (juce::Colours::snow.withAlpha(0.5f));
    g.fillRoundedRectangle (bounds, corner);
    g.drawRoundedRectangle (bounds, corner, 1.0f);

	g.setColour (juce::Colours::black);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds().withTrimmedTop(10), juce::Justification::centredTop, 1);
}

void TransientGroup::resized()
{
	using Track = juce::Grid::TrackInfo;
	using Fr = juce::Grid::Fr;
	using Item = juce::GridItem;

    // const auto rect = getBounds();
    // const auto width = rect.getWidth(), height = rect.getHeight();

    juce::Grid grid;
    grid.templateRows = { Track (Fr (1)) };
    grid.templateColumns = { Track (Fr(1)), Track (Fr(1)) };

    grid.items = {
        Item (mDecayDial), Item(mGainDial)
    };

    constexpr int padding = 4;
    grid.performLayout (getLocalBounds()
        .withTrimmedBottom(padding)
        .withTrimmedTop(padding)
    );
}
