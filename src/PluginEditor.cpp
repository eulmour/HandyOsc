#include "PluginEditor.h"

//==============================================================================
WarpRoomEditor::WarpRoomEditor (WarpRoomAudioProcessor& p,
    std::shared_ptr<juce::AudioProcessorValueTreeState> parameters)
    : AudioProcessorEditor (&p)
    , processorRef (p)
    , mMainComponent(parameters)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    addAndMakeVisible(mMainComponent);
    setResizable(true, true);
    setResizeLimits(384, 240, 3840, 2400);
    getConstrainer()->setFixedAspectRatio(16.0f / 10.0f);
    setSize(640, 400);
}

WarpRoomEditor::~WarpRoomEditor()
{
}

//==============================================================================
void WarpRoomEditor::paint (juce::Graphics& g)
{
    g.fillAll();
}

void WarpRoomEditor::resized()
{
    mMainComponent.setBounds(getLocalBounds());
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}