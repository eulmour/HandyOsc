#include "MainComponent.h"

MainComponent::MainComponent (std::shared_ptr<juce::AudioProcessorValueTreeState> parameters)
	: juce::Component()
	, mParameters (parameters)
    , mMiscGroup (parameters)
    , mTransientGroup(parameters)
{
	setLookAndFeel (&mLnf);
    addAndMakeVisible (mMiscGroup);
    addAndMakeVisible (mTransientGroup);
}

MainComponent::~MainComponent()
{
    setLookAndFeel (nullptr);
}

void MainComponent::resized()
{
	using Track = juce::Grid::TrackInfo;
	using Fr = juce::Grid::Fr;
	using Item = juce::GridItem;

    const auto rect = getBounds();
    const auto width = rect.getWidth(), height = rect.getHeight();

    juce::Grid grid;

    grid.templateRows = { Track (Fr (5)), Track (Fr (1)) };
    grid.templateColumns = { Track (Fr (1)) };
    grid.items = { Item (mTransientGroup), Item(mMiscGroup) };
    grid.performLayout (getLocalBounds());
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::parameterValueChanged (int parameterIndex, float newValue)
{
    mMiscGroup.repaint();
}

void MainComponent::parameterGestureChanged (int parameterIndex, bool gestureIsStarting)
{
    mMiscGroup.repaint();
}
