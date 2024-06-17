#include "Dial.h"
#include "MainLNF.h"

Dial::Dial (std::string const& labelText,
            std::shared_ptr<juce::AudioProcessorValueTreeState> parameters,
            std::string const& parameterId)
	: mDialValue (parameters, parameterId)
	, mSliderAttachment (*parameters, parameterId, mSlider)
{
	mSlider.setSliderStyle (juce::Slider::SliderStyle::RotaryVerticalDrag);
	mSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 80, 20);
	mSlider.setLookAndFeel (&mLookAndFeel);
	mSlider.setVelocityModeParameters (1.0, 1, 0.02, true, juce::ModifierKeys::shiftModifier);
	mSlider.setTextBoxIsEditable (false);
	addAndMakeVisible (mSlider);
	addAndMakeVisible (mDialValue);
	mLabel.setFont (11.0f);
	mLabel.setText (juce::String (labelText), juce::dontSendNotification);
	mLabel.setJustificationType (juce::Justification::centred);
    mLabel.setColour (juce::Label::textColourId, juce::Colours::black);
    setColour (juce::Label::textColourId, juce::Colours::black);
    setColour (juce::Slider::textBoxTextColourId, juce::Colours::black);
    setColour (juce::Slider::thumbColourId, juce::Colours::black);
    setColour (juce::Slider::trackColourId, juce::Colours::black);

	addAndMakeVisible (mLabel);
	updateFontSize();
}

Dial::~Dial()
{
	mSlider.setLookAndFeel (nullptr);
	mLabel.setLookAndFeel (nullptr);
	setLookAndFeel (nullptr);
	mSlider.removeMouseListener (this);
}

void Dial::resized()
{
	juce::Grid grid;
	using Track = juce::Grid::TrackInfo;
	using Fr = juce::Grid::Fr;
	using Item = juce::GridItem;
	grid.templateRows = { Track (Fr (2)), Track (Fr (5)), Track (Fr (1)) };
	grid.templateColumns = { Track (Fr (1)) };
	grid.items = { Item (mLabel), Item (mSlider), Item (mDialValue) };
	grid.performLayout (getLocalBounds());
	updateFontSize();
}

void Dial::updateFontSize()
{
	auto const topLevelBounds = getTopLevelComponent()->getBounds();
	auto const topLevelWidth = topLevelBounds.getWidth();
	auto const topLevelHeight = topLevelBounds.getHeight();
	mLabel.setFont ((topLevelWidth + topLevelHeight) / MainLNF::FontScaleLarge);
}

Dial::LookAndFeel::LookAndFeel()
{
}

Dial::LookAndFeel::~LookAndFeel()
{
}

juce::Slider::SliderLayout Dial::LookAndFeel::getSliderLayout (juce::Slider& slider)
{
	auto const localBounds = slider.getLocalBounds();
	juce::Slider::SliderLayout layout;
	layout.textBoxBounds = localBounds.withY (-1);
	layout.sliderBounds = localBounds;
	return layout;
}

void Dial::LookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
	drawBorder (g, x, y, width, height, sliderPos, rotaryStartAngle, rotaryEndAngle, slider);
	drawStick (g, x, y, width, height, sliderPos, rotaryStartAngle, rotaryEndAngle, slider);
}

void Dial::LookAndFeel::drawBorder (juce::Graphics& g, int x, int y, int width, int height, float, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&)
{
	auto const bounds = juce::Rectangle<float> (x, y, width, height).reduced (2.0f);
	auto const radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
	auto const lineW = radius * 0.085f;
	auto const arcRadius = radius - lineW * 1.6f;

	// Draw border
	juce::Path backgroundArc;
	backgroundArc.addCentredArc(
        bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryStartAngle,
        rotaryEndAngle,
        true);

	g.setColour (MainLNF::BackgroundTertiary);
	g.strokePath (backgroundArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void Dial::LookAndFeel::drawCircle (juce::Graphics& g, int x, int y, int width, int height, float, const float, const float, juce::Slider&)
{
	auto const bounds = juce::Rectangle<float> (x, y, width, height).reduced (2.0f);
	// Draw circle
	auto const circleRadius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.6f;
	g.setColour (MainLNF::BackgroundTertiary);
	g.fillEllipse (juce::Rectangle<float> (
			       bounds.getCentreX() - circleRadius / 2,
			       bounds.getCentreY() - circleRadius / 2,
			       circleRadius,
			       circleRadius));
}

void Dial::LookAndFeel::drawStick (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&)
{
	auto const bounds = juce::Rectangle<float> (x, y, width, height).reduced (2.0f);
	auto const radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
	auto const toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
	auto const lineWidth = radius * 0.085f;
	// Draw stick
	juce::Path stick;
	stick.addRectangle (-lineWidth / 2, (-lineWidth / 2), lineWidth, (radius - lineWidth) * 1.025f);
	g.setColour (MainLNF::BackgroundTertiary);
	g.fillPath (stick, juce::AffineTransform::rotation (toAngle + juce::MathConstants<float>::pi).translated (bounds.getCentre()));
}

