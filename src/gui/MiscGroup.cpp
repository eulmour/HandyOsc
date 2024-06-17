#include "MiscGroup.h"

MiscGroup::MiscGroup (std::shared_ptr<juce::AudioProcessorValueTreeState> parameters)
    : m_MixDial ("Mix", parameters, "Mix")
    , m_GainDial ("Gain", parameters, "OutputGain")
{
    addAndMakeVisible(m_MixDial);
    addAndMakeVisible(m_GainDial);
    setLookAndFeel(&mLookAndFeel);
}

MiscGroup::~MiscGroup()
{
    setLookAndFeel(nullptr);
}

void MiscGroup::paint (juce::Graphics& g)
{
    float thickness = 2.0f;
    constexpr float corner = 10.0f;
    const auto bounds = getLocalBounds().toFloat().reduced (thickness);

    g.setColour (juce::Colours::snow.withAlpha(0.5f));
    g.fillRoundedRectangle (bounds, corner);
    g.drawRoundedRectangle (bounds, corner, 1.0f);
}

void MiscGroup::resized()
{
	using Track = juce::Grid::TrackInfo;
	using Fr = juce::Grid::Fr;
	using Item = juce::GridItem;

    const auto rect = getBounds();
    const auto width = rect.getWidth(), height = rect.getHeight();

    juce::Grid grid;

    grid.templateRows = { Track (Fr (1)) };
    grid.templateColumns = {
        Track (Fr (1)), Track (Fr(1)), Track (Fr(1)), Track (Fr(1)),
        Track (Fr (1)), Track (Fr(1)), Track (Fr(1)), Track (Fr(1)),
        Track (Fr(1)), Track (Fr(1)), Track (Fr(1)), Track (Fr(1))
    };

    grid.items = {
        Item(), Item(), Item(), Item(), Item(), Item(), Item(), Item(), Item(), Item(),
        Item (m_MixDial), Item(m_GainDial)
    };

    constexpr int padding = 4;
    grid.performLayout (getLocalBounds()
        .withTrimmedBottom(padding)
        .withTrimmedTop(padding)
    );
}
